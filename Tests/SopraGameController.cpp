//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "GameController.h"
#include "GameModel.h"

TEST(controller_test, getDistance0) {
    EXPECT_EQ(gameController::getDistance({0, 0}, {0, 0}), -1);
}
TEST(controller_test, getDistance1) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {8, 6}), 0);
}
TEST(controller_test, getDistance2) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {8, 5}), 1);
}
TEST(controller_test, getDistance3) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {8, 7}), 1);
}
TEST(controller_test, getDistance4) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {11, 7}), 3);
}
TEST(controller_test, getDistance5) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {9, 6}), 1);
}
TEST(controller_test, getDistance6) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {9, 7}), 1);
}
TEST(controller_test, getDistance7) {
    EXPECT_EQ(gameController::getDistance({8, 6}, {9, 8}), 2);
}

TEST(controller_test, getAllCrossedCells0) {
    std::vector<gameModel::Position> result1 = gameController::getAllCrossedCells({12, 3}, {14, 4});
    std::vector<gameModel::Position> expected1 = {{13, 3}, {13, 4}};
    EXPECT_EQ(result1.size(), expected1.size());
    EXPECT_EQ(result1[0], expected1[0]);
    EXPECT_EQ(result1[1], expected1[1]);
}

TEST(controller_test, getAllCrossedCells1) {
    std::vector<gameModel::Position> result2 = gameController::getAllCrossedCells({8, 11}, {13, 10});
    std::vector<gameModel::Position> expected2 = {{9, 11}, {10, 11},
                                                  {11, 10}, {12, 10}};
    EXPECT_EQ(result2.size(), expected2.size());
    EXPECT_EQ(result2[0], expected2[0]);
    EXPECT_EQ(result2[1], expected2[1]);
    EXPECT_EQ(result2[2], expected2[2]);
    EXPECT_EQ(result2[3], expected2[3]);
}

TEST(controller_test, getAllCrossedCells2) {
    std::vector<gameModel::Position> result3 = gameController::getAllCrossedCells({8, 0}, {4, 0});
    std::vector<gameModel::Position> expected3 = {{7, 0}, {6, 0},
                                                  {5, 0}};
    EXPECT_EQ(result3.size(), expected3.size());
    EXPECT_EQ(result3[0], expected3[0]);
    EXPECT_EQ(result3[1], expected3[1]);
    EXPECT_EQ(result3[2], expected3[2]);
}