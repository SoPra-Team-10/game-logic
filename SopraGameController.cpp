
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

    auto getDistance(gameModel::Position startPoint, gameModel::Position endPoint,
                     std::shared_ptr<gameModel::Environment> envi) -> int {
        if (envi.get()->getCell(startPoint) == gameModel::Cell::OutOfBounds ||
            envi.get()->getCell(endPoint) == gameModel::Cell::OutOfBounds) return -1;
        if (startPoint == endPoint) return 0;

        int totalDistance = 0;

        int dX = abs(startPoint.x - endPoint.x);
        int dY = abs(startPoint.y - endPoint.y);

        if (dX >= dY) {
            totalDistance += dY;
            totalDistance += dX-dY;
        } else {
            totalDistance += dX;
            totalDistance += dY-dX;
        }

        return totalDistance;
    }
}
