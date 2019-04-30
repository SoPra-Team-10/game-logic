#ifndef GAMELOGIC_SOPRAGAMECONTROLLER_H
#define GAMELOGIC_SOPRAGAMECONTROLLER_H

#include <memory>
#include <valarray>
#include <vector>

#include "SopraGameModel.h"

namespace gameController {

    /**
     * get a vector containing all cells which are crossed by the vector between to cells.
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @return a vector containing all crossed cells.
     */
    auto getAllCrossedCells(gameModel::Position startPoint, gameModel::Position endPoint) ->
        std::vector<gameModel::Position>;

    /**
     * get the ditance between to cells on the game field.
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @return the distance as integer.
     */
    auto getDistance(gameModel::Position startPoint, gameModel::Position endPoint) -> int;
}

#endif //GAMELOGIC_SOPRAGAMECONTROLLER_H
