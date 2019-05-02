#ifndef GAMELOGIC_SOPRAGAMECONTROLLER_H
#define GAMELOGIC_SOPRAGAMECONTROLLER_H

#include <memory>
#include <vector>

#include "GameModel.h"
#include "Action.h"

namespace gameController {
    class Shot;
    class Move;


    /**
     * Generates a random integer in [min, max)
     * @param min
     * @param max
     * @return
     */
     template <typename T>
    T rng(T min, T max);

    /**
     * Rolls the dice and determines whether an action takes place
     * @param actionProbability Probability of the action. Must be between 0 an 1
     * @return true if the action takes place
     */
    bool actionTriggered(double actionProbability);

    /**
     * get a vector containing all cells which are crossed by the vector between to cells. The
     * startPosition and endPosition are NOT included
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @return a vector containing all crossed cells.
     */
    auto getAllCrossedCells(const gameModel::Position &startPoint, const gameModel::Position &endPoint) ->
        std::vector<gameModel::Position>;

    /**
     * get the ditance between to cells on the game field.
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @return the distance as integer.
     */
    auto getDistance(const gameModel::Position &startPoint, const gameModel::Position &endPoint) -> int;

    /**
     * Get all currently possible shots of a given player in a given environment.
     * @param actor actor the acting player as shared pointer.
     * @param envi the selected environment.
     * @return a shots vector
     */
    auto getAllPossibleShots(std::shared_ptr<gameModel::Player> actor, const gameModel::Environment &envi) ->
    std::vector<Shot>;

    /**
     * Get all currently possible moves of a given actor in a given environment
     * @tparam T the actor type (Player or Ball).
     * @param actor actor the acting player or ball as shared pointer.
     * @param envi the selected environment.
     * @return a action vector
     */
    auto getAllPossibleMoves(std::shared_ptr<gameModel::Player> actor, const gameModel::Environment &envi)
    -> std::vector<Move>;

    template <>
    double rng(double min, double max);

    template <>
    int rng(int min, int max);

    template <typename T>
    T rng(T min, T mx){
        static_assert("Only double or int are supported");
        return min;
    }
}


#endif //GAMELOGIC_SOPRAGAMECONTROLLER_H
