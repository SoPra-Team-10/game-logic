#include "SopraGameController.h"

namespace gameController {

    auto getAllCrossedCells(const gameModel::Position &startPoint, const gameModel::Position &endPoint) ->
    std::vector<gameModel::Position> {

        std::vector<gameModel::Position> resultVect;

        // check if cells are valid
        if (gameModel::Environment::getCell(startPoint) == gameModel::Cell::OutOfBounds ||
        gameModel::Environment::getCell(endPoint) == gameModel::Cell::OutOfBounds)
            return resultVect;

        // add the start point to the result
        resultVect.emplace_back(startPoint);

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

        // add the end point to the result
        resultVect.emplace_back(endPoint);

        return resultVect;
    }

    auto getDistance(const gameModel::Position &startPoint, const gameModel::Position &endPoint) -> int {

        // check if cells are valid
        if (gameModel::Environment::getCell(startPoint) == gameModel::Cell::OutOfBounds ||
        gameModel::Environment::getCell(endPoint) == gameModel::Cell::OutOfBounds)
            return -1;

        // check if start and end point are equal
        if (startPoint == endPoint)
            return 0;

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

    template<typename T>
    auto getAllPossibleMoves(std::shared_ptr<T> actor, const gameModel::Environment &envi) -> std::vector<Move<T>> {
        return std::vector<Move<T>>();
    }

    auto getAllPossibleShots(std::shared_ptr<gameModel::Player> actor,
                             const gameModel::Environment &envi) -> std::vector<Shot> {
        return std::vector<Shot>();
    }
}
