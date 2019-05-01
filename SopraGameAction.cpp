#include "SopraGameAction.h"
namespace gameController{
    Action::Action(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) :
        actor(actor), target(target){}


    Shot::Shot(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) :
            Action(actor, target) {}

    void Shot::execute(gameModel::Environment &envi) const{

        if (check(envi) == ActionResult::Impossible)
            return;

        // @ToDo: execute shot
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
        }
        else {
            return ActionResult::Success;
        }
    }

    auto Shot::executeAll(const gameModel::Environment &envi) const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;


        return resultVect;
    }

    auto Shot::rollTheDiceForLandingCell(std::shared_ptr<gameModel::Environment> envi) const -> gameModel::Position{

        // @ToDo: roll the dice ...

        return {};
    }

    auto Shot::getInterceptionPositions(const gameModel::Environment &env) const -> std::vector<gameModel::Position>{
        auto crossedCells = gameController::getAllCrossedCells(this->actor->position, target);
        auto oponentPlayers = env.team1.hasMember(*actor) ? env.team2.getAllPlayers() : env.team2.getAllPlayers();
        std::vector<gameModel::Position> ret;
        for(const auto &cell : crossedCells){
            for(const auto &player : oponentPlayers){
                if(player->position == cell){
                    ret.emplace_back(cell);
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

        /*
        // Foul 1: Flacken
        if ()
            return ActionResult::Foul;

        // Foul 2: Nachtarocken
        if (true)
            return ActionResult::Foul;

        // Foul 3: Stutschen
        if (true)
            return ActionResult::Foul;

        // Foul 4: Keilen
        if (true)
            return ActionResult::Foul;

        // Foul 5: Schlantzeln
        if (true)
            return ActionResult::Foul;

        // no Foul
        return ActionResult::Success;
         */
    }

    auto Move::executeAll(const gameModel::Environment &envi) const ->
        std::vector<std::pair<gameModel::Environment, double>>{

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        return resultVect;
    }

    Move::Move(std::shared_ptr<gameModel::Player> actor, gameModel::Position target): Action(actor, target) {}

    void Move::execute(gameModel::Environment &envi) const {
        this->actor.get()->position = this->target;

        // @ToDo was passiert bei einem Faul???
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
}
