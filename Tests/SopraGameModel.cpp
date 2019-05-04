//
// Created by timluchterhand on 29.04.19.
//

#include <gtest/gtest.h>
#include "GameModel.h"
#include "GameController.h"
#include "setup.h"

TEST(env_test, getCell){
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

TEST(env_test, cellIsFree0){
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

TEST(env_test, cellIsFree1) {
    auto env = setup::createEnv();

    int x = gameController::rng(5, 11);
    int y = gameController::rng(0, 12);

    auto player = env.getPlayer({x, y});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    EXPECT_TRUE(env.cellIsFree(gameModel::Position(x,y)));

    env.team1.keeper->position = gameModel::Position(x,y);

    EXPECT_FALSE(env.cellIsFree(gameModel::Position(x,y)));

}

TEST(env_test, arePlayerInSameTeam) {
    auto env = setup::createEnv();

    auto team1Players = env.team1.getAllPlayers();
    auto team2Players = env.team2.getAllPlayers();

    EXPECT_TRUE(env.arePlayerInSameTeam(*team1Players[gameController::rng(0, 6)], *team1Players[gameController::rng(0, 6)]));
    EXPECT_TRUE(env.arePlayerInSameTeam(*team2Players[gameController::rng(0, 6)], *team2Players[gameController::rng(0, 6)]));
    EXPECT_FALSE(env.arePlayerInSameTeam(*team1Players[gameController::rng(0, 6)], *team2Players[gameController::rng(0, 6)]));
}

TEST(env_test, isPlayerInOwnRestrictedZone) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(3, 6);
    env.team2.keeper->position = gameModel::Position(3, 6);

    EXPECT_TRUE(env.isPlayerInOwnRestrictedZone(*env.team1.keeper));
    EXPECT_FALSE(env.isPlayerInOwnRestrictedZone(*env.team2.keeper));
}

TEST(env_test, isPlayerInOpponentRestrictedZone) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(3, 6);
    env.team2.keeper->position = gameModel::Position(3, 6);

    EXPECT_FALSE(env.isPlayerInOpponentRestrictedZone(*env.team1.keeper));
    EXPECT_TRUE(env.isPlayerInOpponentRestrictedZone(*env.team2.keeper));
}