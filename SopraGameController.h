#ifndef GAMELOGIC_SOPRAGAMECONTROLLER_H
#define GAMELOGIC_SOPRAGAMECONTROLLER_H

#include <vector>
#include <memory>
#include "SopraGameModel.h"

namespace gameController {

    /**
     * get a vector containing all cells which are crossed by the vector between to cells.
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @param envi the selected environment.
     * @return a vector containing all crossed cells.
     */
    auto getAllCrossedCells(gameModel::Position startPoint, gameModel::Position endPoint,
                            std::shared_ptr<gameModel::Environment> envi) ->
    std::vector<std::pair<gameModel::Position, gameModel::Cell>>;

    /**
     * get the ditance between to cells on the game field.
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @param envi the selected environment.
     * @return the distance as integer.
     */
    auto getDistance(gameModel::Position startPoint, gameModel::Position endPoint,
                     std::shared_ptr<gameModel::Environment> envi) -> int;
}

#endif //GAMELOGIC_SOPRAGAMECONTROLLER_H
