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

    auto Shot::execute() const -> std::pair<std::vector<ShotResult>, std::vector<gameModel::Foul>> {
        std::vector<gameModel::Foul> fouls;
        std::vector<ShotResult> shotRes;
        if (check() == ActionCheckResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }

        if(QUAFFLETHROW){
            bool intercept = false;
            for(const auto &pos : getInterceptionPositions()){
                if(actionTriggered(env->config.gameDynamicsProbs.catchQuaffle)){
                    auto interceptPlayer = env->getPlayer(pos);
                    if(!interceptPlayer.has_value()){
                        throw std::runtime_error("No Player at intercept position");
                    }

                    intercept = true;
                    shotRes.push_back(ShotResult::Intercepted);
                    auto &p = interceptPlayer.value();
                    if(INSTANCE_OF(p, gameModel::Chaser) || INSTANCE_OF(p, gameModel::Keeper)){
                        //Quaffle is catched
                        ball->position = pos;
                    } else {
                        //Quaffle bounces off
                        ball->position = pos;
                        moveToAdjacent(ball, env);
                    }
                }
            }

            //Quaffle not intercepted
            if(!intercept){
                auto dist = getDistance(actor->position, target);
                if(actionTriggered(std::pow(env->config.gameDynamicsProbs.throwSuccess, dist))){
                    //Throw successs
                    ball->position = target;
                    shotRes.push_back(ShotResult::ThrowSuccess);
                } else {
                    //Miss -> dispersion
                    shotRes.push_back(ShotResult::Miss);
                    auto possibleCells = getAllLandingCells();
                    if(!possibleCells.empty()){
                        int index = rng(0, static_cast<int>(possibleCells.size()) - 1);
                        ball->position = possibleCells[index];
                    } else {
                        ball->position = target;
                    }
                }
            }

            auto goal = goalCheck();
            if(goal.has_value()){
                shotRes.push_back(goal.value());
            }
        } else if(BLUDGERSHOT){
            auto playerOnTarget = env->getPlayer(target);
            if(playerOnTarget.has_value()){
                //Knock player out an place bludger on random free cell
                if(!INSTANCE_OF(playerOnTarget.value(), gameModel::Beater) &&
                    actionTriggered(env->config.gameDynamicsProbs.knockOut)){
                    shotRes.push_back(ShotResult::Knockout);
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

        return {shotRes, fouls};
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

    auto Shot::check() const -> ActionCheckResult {
        using res = ActionCheckResult;
        if(actor->position == ball->position && gameModel::Environment::getCell(target) != gameModel::Cell::OutOfBounds &&
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

    auto Shot::goalCheck() const -> std::optional<ShotResult> {
        if(target.x == env->quaffle->position.x){
            return {};
        }

        double m = (env->quaffle->position.y - actor->position.y) / static_cast<double>((env->quaffle->position.x - actor->position.x));
        double c = target.y - m * target.x;
        auto passThrough = [this, m, c](const gameModel::Position &p){
            auto left = std::min(target.x, env->quaffle->position.x);
            auto right = std::max(target.x, env->quaffle->position.x);
            if(p.x < left || p.x > right){
                return false;
            }

            auto upper = p.y + 0.5;
            auto lower = p.y - 0.5;
            auto lSide = m * (p.x - 0.5) + c;
            auto rSide = m * (p.x + 0.5) + c;
            return (lSide > lower && lSide < upper) || (rSide > lower && rSide < upper);

        };

        for(const auto &goal : gameModel::Environment::getGoalsLeft()){
            if(passThrough(goal)){
                return ShotResult::ScoreRight;
            }
        }

        for(const auto &goal : gameModel::Environment::getGoalsRight()){
            if(passThrough(goal)){
                return ShotResult::ScoreLeft;
            }
        }

        return {};
    }

    Move::Move(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor, gameModel::Position target):
            Action(std::move(env), std::move(actor), target) {}

    auto Move::check() const -> ActionCheckResult{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1 ||
            this->actor->isFined || this->actor->knockedOut){
            return ActionCheckResult::Impossible;
        }

        if (this->checkForFoul().empty()) {
            return ActionCheckResult::Success;
        }
        else {
            return ActionCheckResult::Foul;
        }
    }

    auto Move::execute() const -> std::pair<std::vector<ShotResult>, std::vector<gameModel::Foul>> {
        std::vector<gameModel::Foul> fouls;
        std::vector<ShotResult> shots;

        // check move
        bool rammingFoulFlag = false;
        ActionCheckResult actionResult = this->check();

        if (actionResult == ActionCheckResult::Impossible) {
            throw std::runtime_error("The selected move is impossible!");
        }
        else if (actionResult == ActionCheckResult::Foul) {
            fouls = this->checkForFoul();
            for (const auto &foul :  fouls) {
                if (foul == gameModel::Foul::Ramming) {
                    rammingFoulFlag = true;
                }
                if (gameController::refereeDecision(foul, this->env->config)) {
                    this->actor->isFined = true;
                }
                if (foul == gameModel::Foul::ChargeGoal) {
                    if (gameModel::Environment::getCell(this->target) == gameModel::Cell::GoalRight) {
                        shots.push_back(ShotResult::ScoreLeft);
                    }
                    else if (gameModel::Environment::getCell(this->target) == gameModel::Cell::GoalLeft) {
                        shots.push_back(ShotResult::ScoreRight);
                    }
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

        return {shots, fouls};
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
