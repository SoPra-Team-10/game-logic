#include <utility>
#include "Action.h"
#define P_TYPE typeid(*actor)
#define B_TYPE typeId(*ball)
#define SEEKER typeid(gameModel::Seeker)
#define KEEPER typeid(gameModel::Keeper)
#define BEATER typeid(gameModel::Beater)
#define CHASER typeid(gameModel::Chaser)
#define QUAFFLE typeid(gameModel::Quaffle)
#define BLUDGER typeid(gameModel::Bludger)

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

        for(const auto &pos : getInterceptionPositions()){
            if(gameController::actionTriggered(env->config.gameDynamicsProbs.catchQuaffle)){
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

    // fertig ?
    auto Shot::check() const -> ActionResult {
        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds) {
            return ActionResult::Impossible;
        }else if(actor->position != ball->position){
            return ActionResult::Impossible;
        } else if(P_TYPE == SEEKER){
            return ActionResult::Impossible;
        }

        return ActionResult::Success;
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
        for(const auto &cell : crossedCells){
            for(const auto &player : env->getOpponents(*actor)){
                if(player->position == cell){
                    ret.emplace_back(cell);
                }
            }
        }

        return ret;
    }

    template <>
    auto Shot::check(gameModel::Chaser &Player, gameModel::Quaffle &Ball) -> ActionResult{
        if(gameModel::Environment::getCell(target) == gameModel::Cell::OutOfBounds ||
            Ball.position != Player.position){
            return ActionResult::Impossible;
        }

        return ActionResult::Success;
    }

    template <>
    auto Shot::check(gameModel::Beater &player, gameModel::Bludger &Ball) -> ActionResult {
        if(player.position != Ball.position){
            return ActionResult::Impossible;
        }

        if(getDistance(player.position, target) > 3){
            return ActionResult::Impossible;
        }

        for(const auto &cell : getAllCrossedCells(player.position, target)){
            if(!env->cellIsFree(cell)){
                return ActionResult::Impossible;
            }
        }

        return ActionResult::Success;
    }

    template <>
    auto Shot::check(gameModel::Keeper &Player, gameModel::Quaffle &Ball) -> ActionResult{
        if(gameModel::Environment::getCell(target) == gameModel::Cell::OutOfBounds ||
           Ball.position != Player.position){
            return ActionResult::Impossible;
        }

        return ActionResult::Success;
    }

    auto Shot::getAllLandingCells() const -> std::vector<gameModel::Position> {
#warning Was tun bei n gerade? Dann lässt sich das Quadrat nicht mittig um target platzieren
        int n = static_cast<int>(std::ceil(gameController::getDistance(actor->position, target) / 7));
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

    // fertig
    auto Move::check() const -> ActionResult{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1) {
            return ActionResult::Impossible;
        }

        if (this->checkForFoul() == gameModel::Foul::None) {
            return ActionResult::Success;
        }
        else {
            return ActionResult::Foul;
        }
    }

    auto Move::executeAll() const ->
        std::vector<std::pair<gameModel::Environment, double>>{

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        // @toDo: da fehlt noch alles

        return resultVect;
    }

    Move::Move(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor, gameModel::Position target):
    Action(std::move(env), std::move(actor), target) {}

    void Move::execute() const {
        ActionResult actionResult = this->check();

        switch (actionResult) {

            case ActionResult::Foul :
                // @ToDo was passiert bei einem Faul???

            case ActionResult::Success :

                if (env->getPlayer(this->target).has_value()) {
                    gameController::movePlayerOnEmptyCell(this->target, this->env);
                }
                this->actor->position = this->target;

            break;

            case ActionResult::Impossible :
                throw std::runtime_error("The Selected move is impossible!");
        }
    }

    // fertig
    auto Move::successProb() const -> double {
        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1){
            return 0;
        }
        else {
            return 1;
        }
    }

    auto Move::checkForFoul() const -> gameModel::Foul {

        // Keilen
        if (env->getPlayer(this->target).has_value() &&
            !env->arePlayerInSameTeam(*(env->getPlayer(this->target).value()), *(this->actor))) {
            return  gameModel::Foul::Keilen;
        }

        // Flacken
        if (env->isPlayerInOwnRestrictedZone(*(this->actor))) {
            if ((env->team1.hasMember(*(this->actor)) && env->getCell(this->target) == gameModel::Cell::GoalLeft) ||
                (env->team2.hasMember(*(this->actor)) && env->getCell(this->target) == gameModel::Cell::GoalRight)) {
                return gameModel::Foul::Flacken;
            }
        }

        // Schnalzeln
        if (typeid(this->actor) != typeid(gameModel::Seeker) &&
                this->actor->position == env->snitch.position) {
            return gameModel::Foul::Schnatzeln;
        }

        if (typeid(this->actor) == typeid(gameModel::Chaser)) {
            // Nachtarocken
            if (env->quaffle.position == this->actor->position) {
                if ((env->team1.hasMember(*(this->actor)) && env->getCell(this->target) == gameModel::Cell::GoalRight) ||
                    (env->team2.hasMember(*(this->actor)) && env->getCell(this->target) == gameModel::Cell::GoalLeft)) {
                    return gameModel::Foul::Nachtarocken;
                }
            }
            // Stutschen
            if (env->team1.hasMember(*(this->actor))) {
                if (env->getCell(this->target) == gameModel::Cell::RestrictedLeft) {
                    const auto &players = env->team1.chasers;

                    for (auto & player : players) {
                        if (env->isPlayerInOpponentRestrictedZone(player)) {
                            return gameModel::Foul::Stutschen;
                        }
                    }
                }
            }
            else {
                if (env->getCell(this->target) == gameModel::Cell::RestrictedRight) {
                    const auto &players = env->team1.chasers;

                    for (auto & player : players) {
                        if (env->isPlayerInOpponentRestrictedZone(player)) {
                            return gameModel::Foul::Stutschen;
                        }
                    }
                }
            }
        }

        return gameModel::Foul::None;
    }
}
