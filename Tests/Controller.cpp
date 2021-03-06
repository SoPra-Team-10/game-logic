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

TEST(controller_test, moveBludger_towards_player) {
    auto env = setup::createEnv();

    env->bludgers[0]->position = gameModel::Position(4, 11);
    env->team1->beaters[0]->position = gameModel::Position(4, 12);

    auto res = gameController::moveBludger(env->bludgers[0], env);
    EXPECT_FALSE(res.has_value());
    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(3, 11));
}

TEST(controller_test, moveBludger_random_no_player) {
    auto env = setup::createEnv();

    env->bludgers[0]->position = gameModel::Position(10, 0);
    env->team1->beaters[0]->isFined = true;
    env->team1->beaters[1]->isFined = true;
    env->team2->beaters[0]->isFined = true;
    env->team2->beaters[1]->isFined = true;


    auto res = gameController::moveBludger(env->bludgers[0], env);
    EXPECT_FALSE(res.has_value());
    EXPECT_THAT(env->bludgers[0]->position, testing::AnyOf(gameModel::Position(9, 0), gameModel::Position(11,0),
                                                           gameModel::Position(9, 1), gameModel::Position(11,1),
                                                           gameModel::Position(10,1)));

}

TEST(controller_test, moveBludger_knock_out) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->bludgers[0]->position = gameModel::Position(3, 10);
    env->quaffle->position = gameModel::Position(2, 10);

    auto res = gameController::moveBludger(env->bludgers[0], env);

    EXPECT_TRUE(res.has_value());
    EXPECT_NE(env->bludgers[0]->position, gameModel::Position(2, 10));
    EXPECT_TRUE(env->team1->chasers[0]->knockedOut);
    EXPECT_FALSE(env->quaffle->position == gameModel::Position(2, 10));
}

TEST(controller_test, moveBludger_no_knock_out) {
    auto env = setup::createEnv();

    env->bludgers[0]->position = gameModel::Position(3, 10);
    env->quaffle->position = gameModel::Position(2, 10);

    auto res = gameController::moveBludger(env->bludgers[0], env);

    EXPECT_TRUE(res.has_value());
    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(2, 10));
    EXPECT_FALSE(env->team1->chasers[0]->knockedOut);
    EXPECT_TRUE(env->quaffle->position == gameModel::Position(2, 10));
}


TEST(controller_test, moveBludger_border){
    auto env = setup::createEnv();
    env->bludgers[0]->position = {16, 8};
    env->team1->keeper->position = {15, 10};
    gameController::moveBludger(env->bludgers[0], env);
    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(15, 9));
}

TEST(controller_test, moveBludger_if_on_player){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->keeper->position;
    gameController::moveBludger(env->bludgers[0], env);
    EXPECT_EQ(env->bludgers[0]->position, env->team1->keeper->position);
}

//------------------------------can perform action test--------------------------------------------------------------------------

TEST(controller_test, can_shoot_test_valid){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->keeper->position;
    EXPECT_TRUE(gameController::playerCanPerformAction(env->team1->keeper, env));
}

TEST(controller_test, can_shoot_test_valid1){
    auto env = setup::createEnv();

    env->bludgers[0]->position = env->team1->beaters[0]->position;
    EXPECT_TRUE(gameController::playerCanPerformAction(env->team1->beaters[0], env));
}

TEST(controller_test, can_shoot_test_no_quaffle){
    auto env = setup::createEnv();

    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->keeper, env));
}

TEST(controller_test, can_shoot_test_wrong_ball){
    auto env = setup::createEnv();

    env->bludgers[0]->position = env->team1->keeper->position;
    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->keeper, env));
}

TEST(controller_test, can_shoot_test_wrong_ball1){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->beaters[0]->position;
    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->beaters[0], env));
}

TEST(controller_test, can_shoot_test_wrong_ball2){
    auto env = setup::createEnv();

    env->snitch->position = env->team1->beaters[0]->position;
    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->beaters[0], env));
}

TEST(controller_test, can_shoot_test_wrong_player){
    auto env = setup::createEnv();

    env->bludgers[0]->position = env->team1->seeker->position;
    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->seeker, env));
}

TEST(controller_test, can_shoot_test_wrong_player1){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->seeker->position;
    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->seeker, env));
}

TEST(controller_test, valid_can_wrest){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->chasers[1]->position;
    env->team2->chasers[2]->position = {8, 4};

    EXPECT_TRUE(gameController::playerCanPerformAction(env->team2->chasers[2], env));
}

TEST(controller_test, valid_can_wrest1){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[2]->position = {12, 10};

    EXPECT_TRUE(gameController::playerCanPerformAction(env->team2->chasers[2], env));
}

TEST(controller_test, valid_can_wrest2){
    auto env = setup::createEnv();
    env->team1->keeper->position = {13, 7};
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[2]->position = {12, 8};

    EXPECT_TRUE(gameController::playerCanPerformAction(env->team2->chasers[2], env));
}

TEST(controller_test, invalid_can_wrest_keeper_protected){
    auto env = setup::createEnv();
    env->team1->keeper->position = {3, 7};
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[2]->position = {4, 7};

    EXPECT_FALSE(gameController::playerCanPerformAction(env->team2->chasers[2], env));
}

TEST(controller_test, invalid_can_wrest_wrong_player){
    auto env = setup::createEnv();
    env->quaffle->position = env->team1->keeper->position;

    EXPECT_FALSE(gameController::playerCanPerformAction(env->team2->keeper, env));
}

TEST(controller_test, invalid_can_wrest_out_of_range){
    auto env = setup::createEnv();
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[2]->position = {10, 10};

    EXPECT_FALSE(gameController::playerCanPerformAction(env->team2->chasers[2], env));
}

TEST(controller_test, invalid_can_wrest_no_one_holding){
    auto env = setup::createEnv();

    EXPECT_FALSE(gameController::playerCanPerformAction(env->team1->chasers[1], env));
}

//-----------------------------------Snitch Move Test-------------------------------------------------------------------

TEST(controller_test, moveSnitch0) {
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position(16, 7);
    env->team1->seeker->position = gameModel::Position(15, 7);
    env->team2->seeker->position = gameModel::Position(0,8);

    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::None);

    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(16, 8), gameModel::Position(16,6), gameModel::Position(15,6),
                                                        gameModel::Position(15,8)));

}

TEST(controller_test, moveSnitch1){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position(11,10);
    env->team1->seeker->position = gameModel::Position(10,9);
    env->team2->seeker->position = {0,8};

    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::None);
    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(10,11), gameModel::Position(11,11),
                                                        gameModel::Position(12,10), gameModel::Position(12,9)));
}

TEST(controller_test, moveSnitch2){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{6,4};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::Stage1);
    EXPECT_EQ(env->snitch->position, gameModel::Position(6,4));
}


TEST(controller_test, moveSnitch3){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{6,4};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::Stage2);
    EXPECT_EQ(env->snitch->position, gameModel::Position(7,5));
}

TEST(controller_test, moveSnitch4){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{7,5};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::Stage2);
    EXPECT_EQ(env->snitch->position, gameModel::Position(8,6));
}

TEST(controller_test, moveSnitch5){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{8,6};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::Stage2);
    EXPECT_EQ(env->snitch->position, gameModel::Position(8,6));
}

TEST(controller_test, moveSnitch6){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->team1->seeker->position = gameModel::Position(10,6);
    env->team2->seeker->position = gameModel::Position(0,6);
    env->snitch->position = gameModel::Position{8,6};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::Stage3);
    EXPECT_EQ(env->snitch->position, gameModel::Position(10,6));
}

TEST(controller_test, moveSnitch7){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{8,6};
    env->team1->seeker->position = gameModel::Position{10,6};
    env->team2->seeker->position = gameModel::Position{6,6};
    env->team1->chasers[1]->position = gameModel::Position{0,6};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::None);
    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(8,5), gameModel::Position(8,7)));
}

TEST(controller_test, moveSnitch8){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{8,6};
    env->team1->seeker->position = gameModel::Position{10,8};
    env->team2->seeker->position = gameModel::Position{6,4};
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::None);
    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(7,7), gameModel::Position(9,5)));
}

TEST(controller_test, moveSnitch9){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{8,6};
    env->team1->seeker->position = gameModel::Position{10,8};
    env->team2->seeker->position = gameModel::Position{6,4};
    env->team1->seeker->isFined = true;
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::None);
    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(7,5), gameModel::Position(7,6), gameModel::Position(7,7),
                                                        gameModel::Position(8,5), gameModel::Position(8,7),
                                                        gameModel::Position(9,5), gameModel::Position(9,6), gameModel::Position(9,7)));
}

TEST(controller_test, moveSnitch10){
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = gameModel::Position{8,6};
    env->team1->seeker->position = gameModel::Position{10,8};
    env->team2->seeker->position = gameModel::Position{6,4};
    env->team1->seeker->isFined = true;
    env->team2->seeker->isFined = true;
    gameController::moveSnitch(env->snitch, env, gameController::ExcessLength::None);
    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(7,5), gameModel::Position(7,6), gameModel::Position(7,7),
                                                      gameModel::Position(8,5), gameModel::Position(8,6), gameModel::Position(8,7),
                                                      gameModel::Position(9,5), gameModel::Position(9,6), gameModel::Position(9,7)));
}

//-----------------------------------Snitch Spawn ----------------------------------------------------------------------

TEST(controller_tets, spawn_snitch){
    auto env = setup::createEnv();
    env->team1->seeker->position = {1, 6};
    env->team2->seeker->position = {15, 6};
    gameController::spawnSnitch(env);
    EXPECT_TRUE(env->snitch->exists);
    EXPECT_EQ(env->snitch->position.x, 8);
}

TEST(controller_tets, spawn_snitch1){
    auto env = setup::createEnv();
    env->team1->seeker->position = {4, 4};
    env->team2->seeker->position = {12, 8};
    gameController::spawnSnitch(env);
    EXPECT_TRUE(env->snitch->exists);
    EXPECT_THAT(env->snitch->position, testing::AnyOf(gameModel::Position(12, 0), gameModel::Position(11, 1),
            gameModel::Position(10, 2), gameModel::Position(9, 3), gameModel::Position(8, 4), gameModel::Position(8, 5),
            gameModel::Position(8, 6), gameModel::Position(8, 7), gameModel::Position(8, 8), gameModel::Position(7, 9),
            gameModel::Position(6, 10), gameModel::Position(5, 11), gameModel::Position(4, 12)));
}

//-----------------------------------Reset Quaffel after Goal-----------------------------------------------------------

TEST(controller_test , moveQuaffelAfterGoal0) {
    auto env = setup::createEnv();

    env->quaffle->position = gameModel::Position(14, 4);
    env->team1->chasers[1]->position = gameModel::Position(8, 6);

    gameController::moveQuaffelAfterGoal(env);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(7,5), gameModel::Position(8,5),
            gameModel::Position(9,5), gameModel::Position(7,6), gameModel::Position(9,6), gameModel::Position(7,7),
            gameModel::Position(8,7), gameModel::Position(9,7)));
}

TEST(controller_test , moveQuaffelAfterGoal1) {
    auto env = setup::createEnv();

    env->quaffle->position = gameModel::Position(14, 4);
    env->bludgers[0]->position = {9, 6};
    env->bludgers[1]->position = {7, 6};

    gameController::moveQuaffelAfterGoal(env);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(8,6));
}

TEST(controller_test , moveQuaffelAfterGoal2) {
    auto env = setup::createEnv();

    env->quaffle->position = gameModel::Position(14, 4);
    env->team2->keeper->position = gameModel::Position(15, 4);

    gameController::moveQuaffelAfterGoal(env);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(15,4));
}

TEST(controller_test , moveQuaffelAfterGoal3) {
    auto env = setup::createEnv();

    env->quaffle->position = gameModel::Position(14, 4);
    env->team2->keeper->position = gameModel::Position(15, 4);
    env->team2->keeper->isFined = true;

    gameController::moveQuaffelAfterGoal(env);
    EXPECT_NE(env->quaffle->position, gameModel::Position(15,4));
}

//-----------------------------------MoveToAdjacent---------------------------------------------------------------------
TEST(controller_test, moveToAdjacentRemoveShit){
    auto env = setup::createEnv();

    env->quaffle->position = {9, 6};
    env->bludgers[0]->position = {9, 5};
    env->bludgers[1]->position = {9, 7};
    env->snitch->exists = true;
    env->snitch->position = {7, 5};
    env->team1->chasers[2]->position = {8, 7};
    env->team1->chasers[0]->position = {7, 6};
    env->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(gameModel::Position(7, 7)));
    env->team1->seeker->position = {8, 6};
    gameController::moveToAdjacent(env->team1->seeker, env);
    EXPECT_EQ(env->team1->seeker->position, gameModel::Position(7, 7));
    EXPECT_TRUE(env->pileOfShit.empty());
}

TEST(controller_test, moveToAdjacentRemoveShitWithBall){
    auto env = setup::createEnv();

    env->team1->seeker->position = {9, 6};
    env->bludgers[0]->position = {9, 5};
    env->bludgers[1]->position = {9, 7};
    env->snitch->exists = true;
    env->snitch->position = {7, 5};
    env->team1->chasers[2]->position = {8, 7};
    env->team1->chasers[0]->position = {7, 6};
    env->quaffle->position = {8, 6};
    env->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(gameModel::Position(7, 7)));
    gameController::moveToAdjacent(env->quaffle, env);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(7, 7));
    EXPECT_TRUE(env->pileOfShit.empty());
}

//---------------------------------getAllPossibleActions----------------------------------------------------------------
TEST(controller_test, getAllPossibleMoves){
    auto env = setup::createEnv();
    auto moves = gameController::getAllPossibleMoves(env->team1->beaters[0], env);
    EXPECT_EQ(moves.size(), 6);
    std::deque<gameModel::Position> poses = {{0, 4}, {1, 4}, {2, 4}, {2, 3}, {1, 2}, {2, 2}};
    for(const auto &move : moves){
        EXPECT_NE(move.check(), gameController::ActionCheckResult::Impossible);
        for(auto it = poses.begin(); it < poses.end(); it++){
            if(*it == move.getTarget()){
                poses.erase(it);
                break;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
}

TEST(controller_test, getAllPossibleShots){
    auto env = setup::createEnv();
    auto shots = gameController::getAllPossibleShots(env->team1->beaters[0], env, 0);
    EXPECT_EQ(shots.size(), 0);
}

TEST(controller_test, getAllPossibleShots1){
    auto env = setup::createEnv();
    env->quaffle->position = env->team2->chasers[1]->position;
    auto shots = gameController::getAllPossibleShots(env->team2->chasers[1], env, 0);
    EXPECT_EQ(shots.size(), 192);
    for(const auto &shot : shots){
        EXPECT_NE(shot.check(), gameController::ActionCheckResult::Impossible);
    }
}

TEST(controller_test, getAllPossibleShots2){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team1->beaters[1]->position;
    auto shots = gameController::getAllPossibleShots(env->team1->beaters[1], env, 0);
    EXPECT_EQ(shots.size(), 18);
    std::deque<gameModel::Position> poses = {{1, 3}, {1, 2}, {2, 3}, {2, 2}, {2, 1}, {3, 3}, {3, 2}, {3, 1},
                                             {4, 2}, {4, 1}, {4, 0}, {5, 2}, {5, 1}, {5, 0}, {6, 3}, {6, 2},
                                             {6, 1}, {6, 0}};
    for(const auto &shot : shots){
        EXPECT_NE(shot.check(), gameController::ActionCheckResult::Impossible);
        for(auto it = poses.begin(); it < poses.end(); it++) {
            if (*it == shot.getTarget()) {
                poses.erase(it);
                break;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
}

TEST(controller_test, getAllPossibleShotsOptimized){
    auto env = setup::createEnv();
    env->quaffle->position = env->team2->chasers[1]->position;
    auto shots = gameController::getAllConstrainedShots(env->team2->chasers[1], env);
    EXPECT_EQ(shots.size(), 49);
    for(const auto &shot : shots){
        EXPECT_NE(shot.check(), gameController::ActionCheckResult::Impossible);
    }
}

TEST(controller_test, getAllPossibleShots2Optimized){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team1->beaters[1]->position;
    auto shots = gameController::getAllConstrainedShots(env->team1->beaters[1], env);
    EXPECT_EQ(shots.size(), 18);
    std::deque<gameModel::Position> poses = {{1, 3}, {1, 2}, {2, 3}, {2, 2}, {2, 1}, {3, 3}, {3, 2}, {3, 1},
                                             {4, 2}, {4, 1}, {4, 0}, {5, 2}, {5, 1}, {5, 0}, {6, 3}, {6, 2},
                                             {6, 1}, {6, 0}};
    for(const auto &shot : shots){
        EXPECT_NE(shot.check(), gameController::ActionCheckResult::Impossible);
        for(auto it = poses.begin(); it < poses.end(); it++) {
            if (*it == shot.getTarget()) {
                poses.erase(it);
                break;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
}
