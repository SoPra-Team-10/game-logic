//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "GameController.h"
#include "GameModel.h"

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

    std::vector<gameModel::Position> result1 = gameController::getAllCrossedCells(gameModel::Position(12, 3), gameModel::Position(14, 4));
    std::vector<gameModel::Position> expected1 = {gameModel::Position(12, 3), gameModel::Position(13, 3), gameModel::Position(13, 4), gameModel::Position(14, 4)};
    EXPECT_EQ(result1.size(), expected1.size());
    EXPECT_EQ(result1[0], expected1[0]);
    EXPECT_EQ(result1[1], expected1[1]);
    EXPECT_EQ(result1[2], expected1[2]);
    EXPECT_EQ(result1[3], expected1[3]);

    std::vector<gameModel::Position> result2 = gameController::getAllCrossedCells(gameModel::Position(8, 11), gameModel::Position(13, 10));
    std::vector<gameModel::Position> expected2 = {gameModel::Position(8, 11), gameModel::Position(9, 11), gameModel::Position(10, 11),
                                                  gameModel::Position(11, 10), gameModel::Position(12, 10), gameModel::Position(13, 10)};
    EXPECT_EQ(result2.size(), expected2.size());
    EXPECT_EQ(result2[0], expected2[0]);
    EXPECT_EQ(result2[1], expected2[1]);
    EXPECT_EQ(result2[2], expected2[2]);
    EXPECT_EQ(result2[3], expected2[3]);
    EXPECT_EQ(result2[4], expected2[4]);
    EXPECT_EQ(result2[5], expected2[5]);

    std::vector<gameModel::Position> result3 = gameController::getAllCrossedCells(gameModel::Position(8, 0), gameModel::Position(4, 0));
    std::vector<gameModel::Position> expected3 = {gameModel::Position(8, 0), gameModel::Position(7, 0), gameModel::Position(6, 0),
                                                  gameModel::Position(5, 0), gameModel::Position(4, 0)};
    EXPECT_EQ(result3.size(), expected3.size());
    EXPECT_EQ(result3[0], expected3[0]);
    EXPECT_EQ(result3[1], expected3[1]);
    EXPECT_EQ(result3[2], expected3[2]);
    EXPECT_EQ(result3[3], expected3[3]);
    EXPECT_EQ(result3[4], expected3[4]);
}
