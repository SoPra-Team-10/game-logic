#include "SopraGameAction.h"
namespace gameController{
    template<class T> Action<T>::Action(const std::shared_ptr<T> actor, const gameModel::Position target) :
        actor(actor), target(target){}


    Shot::Shot(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) :
            Action<gameModel::Player>(actor, target) {}

    void Shot::execute(gameModel::Environment &envi) const{

        if (check(envi) == ActionResult::Impossible)
            return;

        // @ToDo: execute shot
    }

    auto Shot::successProb(const gameModel::Environment &envi) const -> double{

        return pow(envi.config.gameDynamicsProbs.throwSuccess,
                   gameController::getDistance(this->actor.get()->position, this->target));
    }

    auto Shot::check(const gameModel::Environment &envi) const -> ActionResult {

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds)
            return ActionResult::Impossible;
        else
            return ActionResult::Success;
    }

    auto Shot::executeAll(const gameModel::Environment &envi) const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;
        std::vector<Shot> possibleShots = getAllPossibleShots(this->actor, envi);

        for (auto & possibleShot : possibleShots) {

            if (possibleShot.check(envi) == ActionResult::Impossible){
                continue;
            }

            gameModel::Environment testEnvi = envi;
            possibleShot.execute(testEnvi);
            resultVect.emplace_back(std::pair<gameModel::Environment, double>(testEnvi, possibleShot.successProb(envi)));
        }

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

    template<class T> auto Move<T>::check(const gameModel::Environment &envi) const -> ActionResult{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds)
            return ActionResult::Impossible;

        // a move of a ball can't be a Foul
        if ((typeid(this->actor.get()) == typeid(gameModel::Ball)) )
            return ActionResult::Success;

        /*
        // cast to Player
        gameModel::Player player = this->actor.get();

        // Foul 1: Flacken
        if (player.)
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

    template<class T> auto Move<T>::executeAll(const gameModel::Environment &envi) const ->
    std::vector<std::pair<gameModel::Environment, double>>{

        std::vector<std::pair<gameModel::Environment, double>> resultVect;
        std::vector<Move<T>> possibleMoves = getAllPossibleMoves<T>(this->actor, envi);

        for (auto & possibleShot : possibleMoves) {

            if (possibleShot.check(envi) == ActionResult::Impossible){
                continue;
            }

            gameModel::Environment testEnvi = envi;
            possibleShot.execute(testEnvi);
            resultVect.emplace_back(testEnvi, possibleShot.successProb(envi));
        }

        return resultVect;
    }

    template<class T>
    Move<T>::Move(std::shared_ptr<T> actor, gameModel::Position target): Action<T>(actor, target) {}

    template<class T>
    void Move<T>::execute(gameModel::Environment &envi) const {

    }

    template<class T>
    auto Move<T>::successProb(const gameModel::Environment &envi) const -> double {
        return 0;
    }


    template class Move<gameModel::Player>;
    template class Move<gameModel::Ball>;
}
