#ifndef GAMELOGIC_SOPRAGAMECONTROLLER_H
#define GAMELOGIC_SOPRAGAMECONTROLLER_H

#include <memory>
#include <vector>

#include "SopraGameModel.h"
#include "SopraGameAction.h"

namespace gameController {
    class Shot;
    class Move;

    /**
     * get a vector containing all cells which are crossed by the vector between to cells.
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
}


#endif //GAMELOGIC_SOPRAGAMECONTROLLER_H
