//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "GameController.h"
#include "GameModel.h"
#include "setup.h"
#include <gmock/gmock-matchers.h>

//-----------------------------------Get Distance Tests-----------------------------------------------------------------

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

//-----------------------------------Get all Crossed Cells Test---------------------------------------------------------

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

TEST(controller_test, getAllCrossedCells3) {
    std::vector<gameModel::Position> result = gameController::getAllCrossedCells({4, 2}, {13, 6});
    std::vector<gameModel::Position> expected = {{5, 2}, {5, 3}, {6, 3}, {7, 3}, {8, 4},
                                                  {9, 4}, {10, 5}, {11, 5}, {12, 5}, {12, 6}};
    EXPECT_EQ(result.size(), expected.size());
    EXPECT_EQ(result[0], expected[0]);
    EXPECT_EQ(result[1], expected[1]);
    EXPECT_EQ(result[2], expected[2]);
    EXPECT_EQ(result[3], expected[3]);
    EXPECT_EQ(result[4], expected[4]);
    EXPECT_EQ(result[5], expected[5]);
    EXPECT_EQ(result[6], expected[6]);
    EXPECT_EQ(result[7], expected[7]);
    EXPECT_EQ(result[8], expected[8]);
    EXPECT_EQ(result[9], expected[9]);
}

//-----------------------------------Bludger Move Test------------------------------------------------------------------

TEST(controller_test, moveBludger0) {
    auto env = setup::createEnv();

    env->bludgers[0]->position = gameModel::Position(4, 11);
    env->team1->beaters[0]->position = gameModel::Position(4, 12);

    gameController::moveBludger(env->bludgers[0], env);

    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(3, 11));
}

TEST(controller_test, moveBludger1) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    env->bludgers[0]->position = gameModel::Position(3, 10);
    env->quaffle->position = gameModel::Position(2, 10);

    gameController::moveBludger(env->bludgers[0], env);

    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(2, 10));
    EXPECT_TRUE(env->team1->chasers[0]->knockedOut);
    EXPECT_FALSE(env->quaffle->position == gameModel::Position(2, 10));
}

TEST(controller_test, can_shoot_test_valid){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->keeper->position;
    EXPECT_TRUE(gameController::playerCanShoot(env->team1->keeper, env));
}

TEST(controller_test, can_shoot_test_valid1){
    auto env = setup::createEnv();

    env->bludgers[0]->position = env->team1->beaters[0]->position;
    EXPECT_TRUE(gameController::playerCanShoot(env->team1->beaters[0], env));
}

TEST(controller_test, can_shoot_test_no_quaffle){
    auto env = setup::createEnv();

    EXPECT_FALSE(gameController::playerCanShoot(env->team1->keeper, env));
}

TEST(controller_test, can_shoot_test_wrong_ball){
    auto env = setup::createEnv();

    env->bludgers[0]->position = env->team1->keeper->position;
    EXPECT_FALSE(gameController::playerCanShoot(env->team1->keeper, env));
}

TEST(controller_test, can_shoot_test_wrong_ball1){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->beaters[0]->position;
    EXPECT_FALSE(gameController::playerCanShoot(env->team1->beaters[0], env));
}

TEST(controller_test, can_shoot_test_wrong_ball2){
    auto env = setup::createEnv();

    env->snitch->position = env->team1->beaters[0]->position;
    EXPECT_FALSE(gameController::playerCanShoot(env->team1->beaters[0], env));
}

TEST(controller_test, can_shoot_test_wrong_player){
    auto env = setup::createEnv();

    env->bludgers[0]->position = env->team1->seeker->position;
    EXPECT_FALSE(gameController::playerCanShoot(env->team1->seeker, env));
}

TEST(controller_test, can_shoot_test_wrong_player1){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->seeker->position;
    EXPECT_FALSE(gameController::playerCanShoot(env->team1->seeker, env));
}

//-----------------------------------Snitch Move Test------------------------------------------------------------------

TEST(controller_test, moveSnitch0) {
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position(16, 7);
    env->team1->seeker->position = gameModel::Position(15, 7);
    env->team2->seeker->position = {0,8};

    gameController::moveSnitch(env->snitch, env);

    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(16, 8), gameModel::Position(16,6), gameModel::Position(15,6),
                                                        gameModel::Position(15,8)));

}

TEST(controller_test, moveSnitch1){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position(11,10);
    env->team1->seeker->position = gameModel::Position(10,9);
    env->team2->seeker->position = {0,8};

    gameController::moveSnitch(env->snitch, env);

    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(10,11), gameModel::Position(11,11), gameModel::Position(12,11),
                                                        gameModel::Position(12,10), gameModel::Position(12,9) ));
}