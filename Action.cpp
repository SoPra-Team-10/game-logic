#include <utility>
#include "Action.h"
#define P_TYPE typeid(*actor)
#define B_TYPE typeid(*ball)
#define SEEKER typeid(gameModel::Seeker)
#define KEEPER typeid(gameModel::Keeper)
#define BEATER typeid(gameModel::Beater)
#define CHASER typeid(gameModel::Chaser)
#define QUAFFLE typeid(gameModel::Quaffle)
#define BLUDGER typeid(gameModel::Bludger)
#define SNITCH typeid(gameModel::Snitch)
#define QUAFFLETHROW ((P_TYPE == CHASER || P_TYPE == KEEPER) && B_TYPE == QUAFFLE)
#define BLUDGERSHOT (P_TYPE == BEATER && B_TYPE == BLUDGER)

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
                    if(typeid(*p) == CHASER || typeid(*p) == KEEPER){
                        //Quaffle is catched
                        ball->position = pos;
                    } else {
                        //Quaffle bounces off
                        auto possibleCells = env->getAllPlayerFreeCellsAround(pos);
                        int index = rng(0, static_cast<int>(possibleCells.size()));
                        ball->position = possibleCells[index];
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
                int index = rng(0, static_cast<int>(possibleCells.size()));
                ball->position = possibleCells[index];
            }
        } else if(BLUDGERSHOT){
            auto playerOnTarget = env->getPlayer(target);
            if(playerOnTarget.has_value()){
                //Knock player out an place bludger on random free cell
                if(typeid(*playerOnTarget.value()) != BEATER){
                    playerOnTarget.value()->knockedOut = true;
                    auto possibleCells = env->getAllFreeCells();
                    int index = rng(0, static_cast<int>(possibleCells.size()));
                    ball->position = possibleCells[index];
                }
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
                       gameController::getDistance(this->actor.get()->position, this->target));
        }
    }

    auto Shot::check() const -> ActionResult {
        using res = ActionResult;
        if(actor->position == ball->position && gameModel::Environment::getCell(target) != gameModel::Cell::OutOfBounds){
            if(QUAFFLETHROW){
                return res::Success;
            } else if(BLUDGERSHOT){
                if(getDistance(actor->position, target) < 3){
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
        for(const auto &cell : crossedCells){
            for(const auto &player : env->getOpponents(*actor)){
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
            gameController::getDistance(this->actor.get()->position, this->target) > 1){
            return 0;
        }
        else {
            return 1;
        }
    }

    auto Move::checkForFoul() const -> gameModel::Foul {

        // @ToDo: Fouls
        if (env->getPlayer(this->target).has_value() &&
            !env->arePlayerInSameTeam(*(env->getPlayer(this->target).value()), *(this->actor))) {
            return  gameModel::Foul::Flacken;
        }

        if (typeid(this->actor.get()) != typeid(gameModel::Seeker) &&
                this->actor.get()->position == env->snitch.position) {
            return gameModel::Foul::Schnatzeln;
        }


        return gameModel::Foul::None;
    }
}
