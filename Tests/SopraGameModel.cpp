//
// Created by timluchterhand on 29.04.19.
//

#include <gtest/gtest.h>
#include "sopraGameModel.h"

TEST(env_tests, getCell){
    EXPECT_EQ(gameModel::Environment::getCell(6, 3), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(7, 8), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(3, 0), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(3, 1), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(12, 9), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(5, 5), gameModel::Cell::Standard);

    EXPECT_EQ(gameModel::Environment::getCell(7, 7), gameModel::Cell::Centre);
    EXPECT_EQ(gameModel::Environment::getCell(8, 6), gameModel::Cell::Centre);

    EXPECT_EQ(gameModel::Environment::getCell(0, 7), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(3, 8), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(1, 6), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(3, 3), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(2, 11), gameModel::Cell::RestrictedLeft);

    EXPECT_EQ(gameModel::Environment::getCell(14, 1), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(15, 2), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(15, 4), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(13, 9), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(14, 11), gameModel::Cell::RestrictedRight);
}
