//
// Created by tim on 11.06.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Interference.h>
#include "Action.h"
#include "setup.h"

//---------------------------WrestQuaffle Execute Move------------------------------------------------------------------
TEST(wrest_quaffel_test, wrest_execute0) {
    auto env = setup::createEnv();

    env->quaffle->position = env->team2->chasers[0]->position;
    env->team1->chasers[0]->position = gameModel::Position(6,0);

    gameController::WrestQuaffle action(env, env->team1->chasers[0], env->team2->chasers[0]->position);
    auto mvRes = action.execute();

    EXPECT_EQ(env->quaffle->position, env->team2->chasers[0]->position);
    EXPECT_TRUE(mvRes.first.empty());

}

TEST(wrest_quaffel_test, wrest_execute1) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->quaffle->position = env->team2->chasers[0]->position;
    env->team1->chasers[0]->position = gameModel::Position(6,0);

    gameController::WrestQuaffle action(env, env->team1->chasers[0], env->team2->chasers[0]->position);
    auto mvRes = action.execute();

    EXPECT_EQ(env->quaffle->position, gameModel::Position(6,0));
    EXPECT_FALSE(mvRes.first.empty());
    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::WrestQuaffel);

}

TEST(wrest_quaffel_test, wrest_execute2) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->team1->keeper->position = gameModel::Position(0, 4);
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[0]->position = gameModel::Position(0,5);

    gameController::WrestQuaffle action(env, env->team2->chasers[0], env->team1->keeper->position);
    EXPECT_ANY_THROW(action.execute());

}

TEST(wrest_quaffel_test, wrest_execute3) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->team1->keeper->position = gameModel::Position(16, 4);
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[0]->position = gameModel::Position(16,5);

    gameController::WrestQuaffle action(env, env->team2->chasers[0], env->team1->keeper->position);
    auto mvRes = action.execute();

    EXPECT_EQ(env->quaffle->position, gameModel::Position(16,5));
    EXPECT_FALSE(mvRes.first.empty());
    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::WrestQuaffel);

}
