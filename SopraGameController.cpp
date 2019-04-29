
#include "SopraGameController.h"

namespace gameController {
    auto getAllCrossedCells(const gameModel::Position startPoint, const gameModel::Position endPoint,
            const std::shared_ptr<gameModel::Environment> envi) ->
            std::vector<std::pair<gameModel::Position, gameModel::Cell>>  {

        std::vector<std::pair<gameModel::Position, gameModel::Cell>> resultVect;

        if (startPoint == endPoint || envi.get()->getCell(startPoint) == gameModel::Cell::OutOfBounds ||
        envi.get()->getCell(endPoint) == gameModel::Cell::OutOfBounds) return resultVect;

        // @ToDo: calc crossed cells

        return resultVect;
    }
}
