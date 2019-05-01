#include "SopraGameAction.h"
namespace gameController{
    Action::Action(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) :
        actor(actor), target(target){}


    Shot::Shot(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) :
            Action(actor, target) {}

    void Shot::execute(gameModel::Environment &envi) const{
        if (check(envi) == ActionResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }


        for(const auto &pos : getInterceptionPositions(envi)){
            if(gameController::actionTriggered(envi.config.gameDynamicsProbs.catchQuaffle)){
                auto player = envi.getPlayer(pos);
                if(player.has_value()){
                    //@TODO
                } else{
                    throw std::runtime_error("No player at specified interception point");
                }
            }
        }
    }

    auto Shot::successProb(const gameModel::Environment &envi) const -> double{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds){
            return 0;
        }
        else {
            // @ToDo: Wahrscheinlichkeit für das Abfangen einfügen.
            return pow(envi.config.gameDynamicsProbs.throwSuccess,
                       gameController::getDistance(this->actor.get()->position, this->target));
        }
    }

    auto Shot::check(const gameModel::Environment &envi) const -> ActionResult {
        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds) {
            return ActionResult::Impossible;
        } else if(typeid(*actor) == typeid(gameModel::Seeker)){
            return ActionResult::Impossible;
        } else if(typeid(*actor) == typeid(gameModel::Keeper) || typeid(*actor) == typeid(gameModel::Chaser)){
            if(envi.quaffle.position != actor->position){
                return ActionResult::Impossible;
            }
        } else if(typeid(*actor) == typeid(gameModel::Beater)){
            if(envi.bludgers[0].position != actor->position &&
            envi.bludgers[1].position != actor->position){
                return ActionResult::Impossible;
            }
        }

        return ActionResult::Success;
    }

    auto Shot::executeAll(const gameModel::Environment &envi) const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;


        return resultVect;
    }

    auto Shot::getInterceptionPositions(const gameModel::Environment &env) const -> std::vector<gameModel::Position>{
        auto crossedCells = gameController::getAllCrossedCells(this->actor->position, target);
        std::vector<gameModel::Position> ret;
        for(const auto &cell : crossedCells){
            for(const auto &player : env.getOpponents(*actor)){
                if(player->position == cell){
                    ret.emplace_back(cell);
                }
            }
        }

        return ret;
    }

    auto Shot::getAllLandingCells(const gameModel::Environment &env) const -> std::vector<gameModel::Position> {
#warning Was tun bei n gerade? Dann lässt sich das Quadrat nicht mittig um target platzieren
        int n = static_cast<int>(std::ceil(gameController::getDistance(actor->position, target) / 7));
        std::vector<gameModel::Position> ret;
        using Env = gameModel::Environment;
        using Cell = gameModel::Cell;
        auto players = env.getAllPlayers();
        ret.reserve(n * n);
        for(int x = target.x - n / 2; x < target.x + n / 2; x++){
            for(int y = target.y - n / 2; y < target.y + n / 2; y++){
                if(gameModel::Position{x, y} == target){
                    continue;
                }

                if(Env::getCell(x, y) == Cell::OutOfBounds){
                    continue;
                }

                if(env.cellIsFree({x, y})){
                    ret.emplace_back(x, y);
                }
            }
        }

        return ret;
    }

    auto Move::check(const gameModel::Environment &envi) const -> ActionResult{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor.get()->position, this->target) > 1) {
            return ActionResult::Impossible;
        }

        if (this->checkForFoul(envi) == gameModel::Foul::None) {
            return ActionResult::Success;
        }
        else {
            return ActionResult::Foul;
        }
    }

    auto Move::executeAll(const gameModel::Environment &envi) const ->
        std::vector<std::pair<gameModel::Environment, double>>{

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        return resultVect;
    }

    Move::Move(std::shared_ptr<gameModel::Player> actor, gameModel::Position target): Action(actor, target) {}

    void Move::execute(gameModel::Environment &envi) const {
        ActionResult actionResult = this->check(envi);

        switch (actionResult) {

            case ActionResult::Success :
                this->actor.get()->position = this->target;
                break;

            case ActionResult::Foul:
                // @ToDo was passiert bei einem Faul???
                break;

            case ActionResult::Impossible :
                // @ToDo: throw exception
                break;
        }
    }

    auto Move::successProb(const gameModel::Environment &envi) const -> double {
        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor.get()->position, this->target) > 1){
            return 0;
        }
        else {
            return 1;
        }
    }

    auto Move::checkForFoul(const gameModel::Environment &envi) const -> gameModel::Foul {

        return gameModel::Foul::None;
    }
}
