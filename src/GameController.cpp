#include <random>
#include <deque>
#include "GameController.h"

namespace gameController {

    template <>
    double rng(double min, double max){
        static std::random_device rd;
        static std::default_random_engine el(rd());
        std::uniform_real_distribution dist(min, max);
        return dist(el);
    }

    template <>
    int rng(int min, int max){
        static std::random_device rd;
        static std::default_random_engine el(rd());
        std::uniform_int_distribution dist(min, max);
        return dist(el);
    }

    bool actionTriggered(double actionProbability) {
        if(actionProbability < 0 || actionProbability > 1){
            throw std::invalid_argument("Probability not between 0 an 1");
        }

        return rng(0.0, 1.0) < actionProbability;
    }

    auto getAllCrossedCells(const gameModel::Position &startPoint, const gameModel::Position &endPoint) ->
        std::vector<gameModel::Position> {

        std::vector<gameModel::Position> resultVect;

        // check if cells are valid
        if (gameModel::Environment::getCell(startPoint) == gameModel::Cell::OutOfBounds ||
        gameModel::Environment::getCell(endPoint) == gameModel::Cell::OutOfBounds){
            throw std::out_of_range("Source or destination of movement vector are out of bounds");
        }

        // check if start and end point are equal
        if (startPoint == endPoint)
            return resultVect;

        // define an normalize the direction vector
        gameModel::Vector dirVect(endPoint.x - startPoint.x, endPoint.y - startPoint.y);
        dirVect.normalize();

        // traverse the route between the two points
        gameModel::Vector travVect(0, 0);
        gameModel::Position lastCell = startPoint;
        while ((travVect + startPoint) != endPoint) {

            // found a new crossed cell
            if ((travVect + startPoint) != lastCell) {
                lastCell = travVect + startPoint;
                resultVect.emplace_back(lastCell);
            }

            // make a step to travers the vector
            travVect = travVect + (dirVect * 0.5);
        }

        return resultVect;
    }

    auto getDistance(const gameModel::Position &startPoint, const gameModel::Position &endPoint) -> int {

        // check if cells are valid
        if (gameModel::Environment::getCell(startPoint) == gameModel::Cell::OutOfBounds ||
            gameModel::Environment::getCell(endPoint) == gameModel::Cell::OutOfBounds){

            return -1;
        }

        // check if start and end point are equal
        if (startPoint == endPoint){
            return 0;
        }

        int totalDistance = 0;

        // calc the differences within the components of the given points
        int dX = abs(startPoint.x - endPoint.x);
        int dY = abs(startPoint.y - endPoint.y);

        // calculate the total distance
        if (dX >= dY) {
            totalDistance += dY;
            totalDistance += dX-dY;
        } else {
            totalDistance += dX;
            totalDistance += dY-dX;
        }

        return totalDistance;
    }

    void moveToAdjacent(const std::shared_ptr<gameModel::Object> &object, const std::shared_ptr<gameModel::Environment> &env) {
        auto positions = env->getAllFreeCellsAround(object->position);
        object->position = positions[rng(0, static_cast<int>(positions.size()) - 1)];
        if (env->isShitOnCell(object->position)) {
            env->removeShitOnCell(object->position);
        }
    }

    void moveQuaffelAfterGoal(const std::shared_ptr<gameModel::Environment> &env) {
        if (gameModel::Environment::getCell(env->quaffle->position) == gameModel::Cell::GoalRight) {
            if (env->isPlayerInOwnRestrictedZone(env->team2->keeper)) {
                env->quaffle->position = env->team2->keeper->position;
            }
            else if (env->cellIsFree({8, 6})) {
                env->quaffle->position = {8, 6};
            }
            else {
                env->quaffle->position = {8, 6};
                gameController::moveToAdjacent(env->quaffle, env);
            }
        }
        else if (gameModel::Environment::getCell(env->quaffle->position) == gameModel::Cell::GoalLeft) {
            if (env->isPlayerInOwnRestrictedZone(env->team1->keeper)) {
                env->quaffle->position = env->team1->keeper->position;
            }
            else if (env->cellIsFree({8, 6})) {
                env->quaffle->position = {8, 6};
            }
            else {
                env->quaffle->position = {8, 6};
                gameController::moveToAdjacent(env->quaffle, env);
            }
        }
    }

    auto getAllPossibleMoves(std::shared_ptr<gameModel::Player>, const gameModel::Environment&) -> std::vector<Move> {
        return std::vector<Move>();
        // @ToDo: Nicht für Server relevant
    }

    auto getAllPossibleShots(std::shared_ptr<gameModel::Player>,
                             const gameModel::Environment&) -> std::vector<Shot> {
        return std::vector<Shot>();
        // @ToDo: Nicht für Server relevant
    }

    auto refereeDecision(const gameModel::Foul &foul, const gameModel::Config &gameConf) -> bool {
        return actionTriggered(gameConf.getFoulDetectionProb(foul));
    }

    // ToDo: TESTS!!!
    auto moveBludger(std::shared_ptr<gameModel::Bludger> &bludger, std::shared_ptr<gameModel::Environment> &env)
        -> std::optional<std::shared_ptr<gameModel::Player>> {
        auto players = env->getAllPlayers();

        // find nearest player
        int minDistance = std::numeric_limits<int>::max();
        std::vector<std::shared_ptr<gameModel::Player>> minDistancePlayers;
        for (const auto &player: players) {

            if (!INSTANCE_OF(player, gameModel::Beater) && !player->isFined) {
                if (getDistance(bludger->position, player->position) < minDistance) {
                    minDistance = getDistance(bludger->position, player->position);
                    minDistancePlayers.clear();
                    minDistancePlayers.emplace_back(player);
                }
                else if (getDistance(bludger->position, player->position) == minDistance) {
                    minDistancePlayers.emplace_back(player);
                }
            }
        }

        if (minDistancePlayers.empty()) {
            gameController::moveToAdjacent(bludger, env);
        }
        else {
            auto minDistancePlayer = minDistancePlayers[rng(0, static_cast<int>(minDistancePlayers.size() - 1))];

            // move towards nearest player
            auto crossedCells = getAllCrossedCells(bludger->position, minDistancePlayer->position);
            if (crossedCells.empty()) {
                // bludger will move on the players position
                bludger->position = minDistancePlayer->position;

                // roll the dices
                if (actionTriggered(env->config.gameDynamicsProbs.knockOut)) {
                    // quaffel test
                    if (env->quaffle->position == minDistancePlayer->position) {
                        moveToAdjacent(env->quaffle, env);
                    }
                    // knockout player
                    minDistancePlayer->knockedOut = true;

                    //Set Bludger to new random position
                    auto possiblePositions = env->getAllFreeCells();
                    bludger->position = possiblePositions[rng(0, static_cast<int>(possiblePositions.size() - 1))];
                }

                return minDistancePlayer;
            }
            else {
                // move in the direction of the nearest player
                bludger->position = crossedCells[0];
            }
        }
        return {};
    }

    bool playerCanPerformAction(const std::shared_ptr<const gameModel::Player> &player,
                                const std::shared_ptr<const gameModel::Environment> &env) {
        if(player->knockedOut || player->isFined){
            return false;
        }

        // check if there is generally allowed to perform a shot and a ball to shot on the same position as the player
        if(player->position == env->quaffle->position && (INSTANCE_OF(player, const gameModel::Chaser) ||
            INSTANCE_OF(player, const gameModel::Keeper))){
            return true;
        } else if((player->position == env->bludgers[0]->position || player->position == env->bludgers[1]->position) &&
                    INSTANCE_OF(player, const gameModel::Beater)){
            return true;
        }

        // check if player can wrest quaffel
        auto playerHoldingQuaffle = env->getPlayer(env->quaffle->position);
        if(INSTANCE_OF(player, const gameModel::Chaser) && getDistance(player->position, env->quaffle->position) == 1 &&
            playerHoldingQuaffle.has_value() && !playerHoldingQuaffle.value()->isFined &&
            !env->arePlayerInSameTeam(player, playerHoldingQuaffle.value()) &&
            (INSTANCE_OF(playerHoldingQuaffle.value(), gameModel::Chaser) ||
            (INSTANCE_OF(playerHoldingQuaffle.value(), gameModel::Keeper) &&
            !env->isPlayerInOwnRestrictedZone(playerHoldingQuaffle.value())))){
            return true;
        }

        return false;
    }

    bool moveSnitch(std::shared_ptr<gameModel::Snitch> &snitch, std::shared_ptr<gameModel::Environment> &env, ExcessLength excessLength){
        if (!snitch->exists) {
            throw std::runtime_error("Snitch does not exist");
        }

        std::deque<gameModel::Position> possiblePositions;
        int minDistanceSeeker = std::numeric_limits<int>::max();
        if(!env->team1->seeker->isFined){
            minDistanceSeeker = getDistance(snitch->position, env->team1->seeker->position);
        }
        auto closestSeeker = env->team1->seeker;
        bool equalDistance = false;
        if(minDistanceSeeker == getDistance(snitch->position, env->team2->seeker->position) && !env->team2->seeker->isFined){
            equalDistance = true;
        } else if(minDistanceSeeker > getDistance(snitch->position, env->team2->seeker->position) && !env->team2->seeker->isFined){
            minDistanceSeeker = getDistance(snitch->position, env->team2->seeker->position);
            closestSeeker = env->team2->seeker;
        }
        switch (excessLength) {
            case ExcessLength::None : {
                if(env->team1->seeker->isFined && env->team2->seeker->isFined){
                    moveToAdjacent(env->snitch, env);
                    return false;
                }
                std::vector<gameModel::Position> freeCells = env->getAllFreeCellsAround(snitch->position);
                for(const auto &pos : freeCells){
                    if((!equalDistance && getDistance(pos, closestSeeker->position) > minDistanceSeeker) ||
                    (equalDistance && getDistance(pos, env->team1->seeker->position) > minDistanceSeeker &&
                    getDistance(pos, env->team2->seeker->position) > minDistanceSeeker)){
                        possiblePositions.emplace_back(pos);
                    }
                }

                if(possiblePositions.empty()){
                    for(const auto &pos : freeCells){
                        if((!equalDistance && getDistance(pos, closestSeeker->position) >= minDistanceSeeker) ||
                           (equalDistance && getDistance(pos, env->team1->seeker->position) >= minDistanceSeeker &&
                            getDistance(pos, env->team2->seeker->position) >= minDistanceSeeker)){
                            possiblePositions.emplace_back(pos);
                        }
                    }
                }

                if (possiblePositions.empty()) {
                    snitch->position = freeCells[rng(0, static_cast<int>(freeCells.size() - 1))];
                } else {
                    snitch->position = possiblePositions[rng(0, static_cast<int>(possiblePositions.size() - 1))];
                }
            }

                return false;
            case ExcessLength::Stage1:
                return false;
            case ExcessLength::Stage2: {
                if(env->team1->seeker->isFined && env->team2->seeker->isFined){
                    moveToAdjacent(env->snitch, env);
                    return false;
                }
                std::vector<gameModel::Position> newPosition = getAllCrossedCells(snitch->position,
                                                                                  gameModel::Position(8, 6));
                if (newPosition.empty()) {
                    snitch->position = gameModel::Position{8,6};
                }else{
                    snitch->position = newPosition[0];
                }

                auto playerOnSnitch = env->getPlayer(snitch->position);
                if(playerOnSnitch.has_value() && !playerOnSnitch.value()->isFined){
                    if(INSTANCE_OF(playerOnSnitch.value(), gameModel::Seeker)){
                        env->getTeam(playerOnSnitch.value())->score += 30;
                        return true;
                    } else{
                        return false;
                    }
                } else {
                    return false;
                }
            }
            case ExcessLength::Stage3: {
                if(env->team1->seeker->isFined && env->team2->seeker->isFined){
                    moveToAdjacent(env->snitch, env);
                    return false;
                }
                snitch->position = closestSeeker->position;
                env->getTeam(closestSeeker)->score += 30;
                return true;
            }
            default:
                throw std::runtime_error("Fatal error! Enum out of bounds");
        }
    }

    void spawnSnitch(std::shared_ptr<gameModel::Environment> &env){
        gameModel::Vector dirVec = {env->team1->seeker->position, env->team2->seeker->position};
        dirVec = dirVec * 0.5;
        gameModel::Vector dirVectOrtho = dirVec.orthogonal();
        dirVectOrtho.normalize();
        std::vector<gameModel::Position> resultVect1;
        std::vector<gameModel::Position> resultVect2;
        gameModel::Position lowerCell = {static_cast<int>(env->team1->seeker->position.x + dirVec.x), static_cast<int>(env->team1->seeker->position.y + dirVec.y)};
        gameModel::Position upperCell = lowerCell;
        bool notOutOfBounds1;
        bool notOutOfBounds2;
        while((notOutOfBounds1 = env->getCell(lowerCell.x + static_cast<int>(std::round(dirVectOrtho.x)), lowerCell.y + static_cast<int>(std::round(dirVectOrtho.y))) != gameModel::Cell::OutOfBounds) ||
                (notOutOfBounds2 = env->getCell(upperCell.x - static_cast<int>(std::round(dirVectOrtho.x)), upperCell.y - static_cast<int>(std::round(dirVectOrtho.y))) != gameModel::Cell::OutOfBounds)){
            if(notOutOfBounds1) {
                lowerCell.x = lowerCell.x + static_cast<int>(std::round(dirVectOrtho.x));
                lowerCell.y = lowerCell.y + static_cast<int>(std::round(dirVectOrtho.y));
                resultVect1.emplace_back(lowerCell);
            }
            if(notOutOfBounds2) {
                upperCell.x = upperCell.x - static_cast<int>(std::round(dirVectOrtho.x));
                upperCell.y = upperCell.y - static_cast<int>(std::round(dirVectOrtho.y));
                resultVect2.emplace_back(upperCell);
            }
        }
        if(resultVect1.size() >= resultVect2.size()) {
            if(env->cellIsFree(resultVect1.back())) {
                env->snitch->position = resultVect1.back();
            }else{
                moveToAdjacent(env->snitch, env);
            }
        }else {
            if(env->cellIsFree(resultVect2.back())) {
                env->snitch->position = resultVect2.back();
            }else{
                moveToAdjacent(env->snitch, env);
            }
        }

        env->snitch->exists = true;
    }
}