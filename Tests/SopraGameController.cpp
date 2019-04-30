//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "SopraGameController.h"
#include "SopraGameModel.h"

TEST(controller_test, getDistance) {
    EXPECT_EQ(gameController::getDistance(gameModel::Position(0, 0), gameModel::Position(0, 0)), -1);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(8, 6)), 0);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(8, 5)), 1);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(8, 7)), 1);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(11, 7)), 3);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(9, 6)), 1);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(9, 7)), 1);
    EXPECT_EQ(gameController::getDistance(gameModel::Position(8, 6), gameModel::Position(9, 8)), 2);
}

TEST(controller_test, getAllCrossedCells) {

    std::vector<gameModel::Position> result = gameController::getAllCrossedCells(gameModel::Position(12, 3), gameModel::Position(14, 4));
    std::vector<gameModel::Position> expected = {gameModel::Position(12, 3), gameModel::Position(13, 3), gameModel::Position(13, 4), gameModel::Position(14, 4)};
    EXPECT_EQ(result.size(), expected.size());
    EXPECT_EQ(result[0], expected[0]);
    EXPECT_EQ(result[1], expected[1]);
    EXPECT_EQ(result[2], expected[2]);
    EXPECT_EQ(result[3], expected[3]);

}
