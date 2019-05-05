#include <utility>
#include "Action.h"
#include "GameModel.h"
#define QUAFFLETHROW ((INSTANCE_OF(actor, gameModel::Chaser) || INSTANCE_OF(actor, gameModel::Keeper)) && INSTANCE_OF(ball, gameModel::Quaffle))
#define BLUDGERSHOT (INSTANCE_OF(actor, gameModel::Beater) && INSTANCE_OF(ball, gameModel::Bludger))

namespace gameController{
    Action::Action(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
            gameModel::Position target) : env(std::move(env)), actor(std::move(actor)), target(target){}


    Shot::Shot(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
            std::shared_ptr<gameModel::Ball> ball, gameModel::Position target) :
            Action(std::move(env), std::move(actor), target), ball(std::move(ball)) {}

    void Shot::execute() const{
        if (check() == ActionResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }

        if(QUAFFLETHROW){
            for(const auto &pos : getInterceptionPositions()){
                if(actionTriggered(env->config.gameDynamicsProbs.catchQuaffle)){
                    auto interceptPlayer = env->getPlayer(pos);
                    if(!interceptPlayer.has_value()){
                        throw std::runtime_error("No Player at intercept position");
                    }

                    auto &p = interceptPlayer.value();
                    if(INSTANCE_OF(p, gameModel::Chaser) || INSTANCE_OF(p, gameModel::Keeper)){
                        //Quaffle is catched
                        ball->position = pos;
                        return;
                    } else {
                        //Quaffle bounces off
                        ball->position = pos;
                        moveToAdjacent(ball, env);
                        return;
                    }
                }
            }

            //Quaffle not intercepted

            auto dist = getDistance(actor->position, target);
            if(actionTriggered(std::pow(env->config.gameDynamicsProbs.throwSuccess, dist))){
                //Throw successs
                ball->position = target;
            } else {
                //Miss -> dispersion
                auto possibleCells = getAllLandingCells();
                if(!possibleCells.empty()){
                    int index = rng(0, static_cast<int>(possibleCells.size()) - 1);
                    ball->position = possibleCells[index];
                } else {
                    ball->position = target;
                }
            }
        } else if(BLUDGERSHOT){
            auto playerOnTarget = env->getPlayer(target);
            if(playerOnTarget.has_value()){
                //Knock player out an place bludger on random free cell
                if(!INSTANCE_OF(playerOnTarget.value(), gameModel::Beater) &&
                    actionTriggered(env->config.gameDynamicsProbs.knockOut)){
                    playerOnTarget.value()->knockedOut = true;
                    auto possibleCells = env->getAllFreeCells();
                    int index = rng(0, static_cast<int>(possibleCells.size()) - 1);
                    ball->position = possibleCells[index];

                    //fool quaffle away
                    if(env->quaffle->position == target){
                        moveToAdjacent(env->quaffle, env);
                    }
                } else {
                    ball->position = target;
                }
            } else {
                ball->position = target;
            }
        }
    }

    auto Shot::successProb() const -> double{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds){
            return 0;
        }
        else {
            // @ToDo: Wahrscheinlichkeit für das Abfangen einfügen.
            return std::pow(env->config.gameDynamicsProbs.throwSuccess,
                       gameController::getDistance(this->actor->position, this->target));
        }
    }

    auto Shot::check() const -> ActionResult {
        using res = ActionResult;
        if (actor->position == ball->position && gameModel::Environment::getCell(target) != gameModel::Cell::OutOfBounds &&
            !actor->isFined && !actor->knockedOut){
            if(QUAFFLETHROW){
                return res::Success;
            } else if(BLUDGERSHOT){
                if(getDistance(actor->position, target) <= 3){
                    bool blocked = false;
                    for(const auto &cell : getAllCrossedCells(actor->position, target)){
                        if(!env->cellIsFree(cell)){
                            blocked = true;
                            break;
                        }
                    }

                    if(!blocked){
                        return res::Success;
                    }
                }
            }
        }

        return res::Impossible;
    }

    auto Shot::executeAll() const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        // @toDo: da fehlt noch alles

        return resultVect;
    }

    auto Shot::getInterceptionPositions() const -> std::vector<gameModel::Position>{
        auto crossedCells = gameController::getAllCrossedCells(this->actor->position, target);
        std::vector<gameModel::Position> ret;
        for(const auto &player : env->getOpponents(actor)){
            for(const auto &cell : crossedCells){
                if(player->position == cell){
                    ret.emplace_back(cell);
                }
            }
        }

        return ret;
    }

    auto Shot::getAllLandingCells() const -> std::vector<gameModel::Position> {
#warning Was tun bei n gerade? Dann lässt sich das Quadrat nicht mittig um target platzieren
        int n = static_cast<int>(std::ceil(getDistance(actor->position, target) / 7.0));
        std::vector<gameModel::Position> ret;
        using Env = gameModel::Environment;
        using Cell = gameModel::Cell;
        auto players = env->getAllPlayers();
        ret.reserve(n * n);
        for(int x = target.x - n / 2; x <= target.x + n / 2; x++){
            for(int y = target.y - n / 2; y <= target.y + n / 2; y++){
                if(gameModel::Position{x, y} == target){
                    continue;
                }

                if(Env::getCell(x, y) == Cell::OutOfBounds){
                    continue;
                }

                if(env->cellIsFree({x, y})){
                    ret.emplace_back(x, y);
                }
            }
        }

        return ret;
    }

    Move::Move(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor, gameModel::Position target):
            Action(std::move(env), std::move(actor), target) {}

    auto Move::check() const -> ActionResult{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1 ||
            this->actor->isFined || this->actor->knockedOut) {
            return ActionResult::Impossible;
        }

        if (this->checkForFoul().empty()) {
            return ActionResult::Success;
        }
        else {
            return ActionResult::Foul;
        }
    }

    void Move::execute() const {
        // check move
        bool rammingFoulFlag = false;
        ActionResult actionResult = this->check();
        if (actionResult == ActionResult::Impossible) {
            throw std::runtime_error("The selected move is impossible!");
        }
        else if (actionResult == ActionResult::Foul) {
            std::vector<gameModel::Foul> fouls = this->checkForFoul();
            for (const auto &foul :  fouls) {
                if (foul == gameModel::Foul::Ramming) {
                    rammingFoulFlag = true;
                }
                if (gameController::refereeDecision(foul, this->env->config)) {
                    this->actor->isFined = true;
                }

            }
        }

        // get other player on target cell
        auto targetPlayer = env->getPlayer(this->target);

        // move the player
        auto oldActorPos = actor->position;
        this->actor->position = this->target;

                // move the quaffel if necessary
        if (this->env->quaffle->position == oldActorPos) {
            this->env->quaffle->position = this->target;
        } else if(rammingFoulFlag && env->quaffle->position == target) {
            //rammed player looses quaffel
            moveToAdjacent(env->quaffle, env);
        }

        // move other player out of the way if necessary
        if (targetPlayer.has_value()) {
            gameController::moveToAdjacent(targetPlayer.value(), env);
        }
    }

    auto Move::successProb() const -> double {
        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1){
            return 0;
        }
        else {
            return 1;
        }
    }

    auto Move::checkForFoul() const -> std::vector<gameModel::Foul> {
        std::vector<gameModel::Foul> resVect;
        // Ramming
        auto player = env->getPlayer(target);
        if (player.has_value() && !env->arePlayerInSameTeam(player.value(), this->actor)) {
            resVect.emplace_back(gameModel::Foul::Ramming);
        }

        // BlockGoal
        if ((env->team1->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalLeft) ||
            (env->team2->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalRight)) {
            resVect.emplace_back(gameModel::Foul::BlockGoal);
        }

        // BlockSnitch
        if (!INSTANCE_OF(actor, gameModel::Seeker) && this->target == this->env->snitch->position &&
                env->snitch->exists) {
            resVect.emplace_back(gameModel::Foul::BlockSnitch);
        }

        if (INSTANCE_OF(actor, gameModel::Chaser)) {

            // ChargeGoal
            if (env->quaffle->position == this->actor->position) {
                if ((env->team1->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalRight) ||
                    (env->team2->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalLeft)) {
                    resVect.emplace_back(gameModel::Foul::ChargeGoal);
                }
            }

            // MultipleOffence
            auto origPos = actor->position;
            actor->position = target;
            if(env->isPlayerInOpponentRestrictedZone(actor)){
                auto mates = env->getTeamMates(actor);
                for(const auto &p : mates){
                    if(INSTANCE_OF(p, gameModel::Chaser) && env->isPlayerInOpponentRestrictedZone(p)){
                        resVect.emplace_back(gameModel::Foul::MultipleOffence);
                    }
                }
            }

            actor->position = origPos;
        }

        return resVect;
    }

    auto Move::executeAll() const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        // @toDo: da fehlt noch alles

        return resultVect;
    }
}
