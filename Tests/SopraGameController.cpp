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
