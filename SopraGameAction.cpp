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
                    if(typeid(player.value()) == typeid(gameModel::Chaser)){
                    }
                } else{
                    throw std::runtime_error("No player at specified interception point");
                }
            }
        }
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

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds){
            return ActionResult::Impossible;
        }

        // a move of a ball can't be a Foul
        if ((typeid(this->actor.get()) == typeid(gameModel::Ball)) ){
            return ActionResult::Success;
        }

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

   auto Move::executeAll(const gameModel::Environment &envi) const ->
    std::vector<std::pair<gameModel::Environment, double>>{

        std::vector<std::pair<gameModel::Environment, double>> resultVect;
        std::vector<Move> possibleMoves = getAllPossibleMoves(this->actor, envi);

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

    Move::Move(std::shared_ptr<gameModel::Player> actor, gameModel::Position target): Action(actor, target) {}

    void Move::execute(gameModel::Environment &envi) const {

    }

    auto Move::successProb(const gameModel::Environment &envi) const -> double {
        return 0;
    }
}
