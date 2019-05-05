//
// Created by timluchterhand on 29.04.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "GameModel.h"
#include "setup.h"

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

TEST(env_tests, cellIsFree){
    auto env = setup::createEnv();

    EXPECT_FALSE(env.cellIsFree(env.team1.chasers[0]->position));
    EXPECT_FALSE(env.cellIsFree(env.team1.chasers[1]->position));
    EXPECT_FALSE(env.cellIsFree(env.team1.chasers[2]->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.chasers[0]->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.chasers[1]->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.chasers[2]->position));
    EXPECT_FALSE(env.cellIsFree(env.team1.beaters[0]->position));
    EXPECT_FALSE(env.cellIsFree(env.team1.beaters[1]->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.beaters[0]->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.beaters[1]->position));
    EXPECT_FALSE(env.cellIsFree(env.team1.keeper->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.keeper->position));
    EXPECT_FALSE(env.cellIsFree(env.team1.seeker->position));
    EXPECT_FALSE(env.cellIsFree(env.team2.seeker->position));

    EXPECT_TRUE(env.cellIsFree({8, 6}));
    EXPECT_TRUE(env.cellIsFree({2, 6}));
    EXPECT_TRUE(env.cellIsFree({2, 9}));
    EXPECT_TRUE(env.cellIsFree({11, 7}));
    EXPECT_TRUE(env.cellIsFree({5, 10}));
    EXPECT_TRUE(env.cellIsFree({4, 1}));
    EXPECT_TRUE(env.cellIsFree({10, 2}));
}

TEST(env_tests, getSurroundingPositions){
    using env = gameModel::Environment;
    using P = gameModel::Position;
    for(const auto &pos : env::getSurroundingPositions({10, 2})){
        EXPECT_THAT(pos, testing::AnyOf(P{10, 1}, P{9, 1}, P{9, 2}, P{9, 3}, P{10, 3}, P{11, 3}, P{11, 2}, P{11, 1}));
    }
}

TEST(env_tests, getSurroundingPositions_corner){
    using env = gameModel::Environment;
    using P = gameModel::Position;
    for(const auto &pos : env::getSurroundingPositions({2, 1})){
        EXPECT_THAT(pos, testing::AnyOf(P{1, 2}, P{2, 2}, P{3, 2}, P{3, 1}, P{3, 0}));
    }
}
