//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "Action.h"
#include "setup.h"
//-----------------------------------Throw checks-----------------------------------------------------------------------

//OOB throw_check
TEST(shot_test, oob_throw_check) {
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

//Valid Keeper throw_check
TEST(shot_test, valid_keeper_throw_check) {
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Success);
}

//Invalid Keeper throw_check (Ball not at keeper position)
TEST(shot_test, inv_keeper_throw_check){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

//Valid Chaser throw_check
TEST(shot_test, valid_chaser_throw_check){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Shot testShot(env, env->team1->chasers[0], env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Success);
}

TEST(shot_test, inv_chaser_throw_check_banned){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->chasers[0]->position;
    env->team1->chasers[0]->isFined = true;
    gameController::Shot testShot(env, env->team1->chasers[0], env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

TEST(shot_test, inv_chaser_throw_check_knocked_out){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->chasers[0]->position;
    env->team1->chasers[0]->knockedOut = true;
    gameController::Shot testShot(env, env->team1->chasers[0], env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

//Invalid Seeker throw_check
TEST(shot_test, inv_seeker_throw_check){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->seeker->position;
    gameController::Shot testShot(env, env->team1->seeker, env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

//Invalid Beater throw_check
TEST(shot_test, inv_beater_throw_check){
    auto env = setup::createEnv();

    env->quaffle->position = env->team1->beaters[0]->position;
    gameController::Shot testShot(env, env->team1->beaters[0], env->quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

//-----------------Throw execute----------------------------------------------------------------------------------------

//Keeper throw_checks ball to centre, no intercept, 100% chance of success
TEST(shot_test, success_throw_execute){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(8, 6));
}

//Keeper throw_checks ball to centre, intercepted
TEST(shot_test, throw_execute_intercept){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 1, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[1]->position = {9, 7};
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::Intercepted);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(9, 7));
}

TEST(shot_test, throw_execute_intercept_bounce_off){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 1, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    env->team2->beaters[0]->position = {10, 9};
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::Intercepted);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(10, 8), gameModel::Position(9, 8),
            gameModel::Position(9, 10), gameModel::Position(10, 10), gameModel::Position(11, 10), gameModel::Position(11, 9)));
}

TEST(shot_test, throw_execute_fail_and_disperse){
    auto env = setup::createEnv({0, {}, {}, {0, 0, 0, 0, 0, 0}, {}});

    env->team1->keeper->position = {0, 8};
    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {16, 8});
    auto res = testShot.execute();
    EXPECT_GE(res.first.size(), 2);
    EXPECT_EQ(res.first[0], gameController::ShotResult::Miss);
    EXPECT_EQ(res.first[1], gameController::ShotResult::ScoreRight);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(13,11), gameModel::Position(14,11),
                                                      gameModel::Position(13, 10), gameModel::Position(14, 10), gameModel::Position(15 ,10),
                                                      gameModel::Position(13, 9), gameModel::Position(14, 9), gameModel::Position(15, 9),
                                                      gameModel::Position(13, 8), gameModel::Position(14, 8), gameModel::Position(15, 8), gameModel::Position(16, 7),
                                                      gameModel::Position(13, 7), gameModel::Position(14, 7), gameModel::Position(15, 7), gameModel::Position(16, 7),
                                                      gameModel::Position(13, 6), gameModel::Position(14, 6), gameModel::Position(15, 6), gameModel::Position(16, 6),
                                                      gameModel::Position(13, 5), gameModel::Position(14, 5), gameModel::Position(15, 5), gameModel::Position(16, 5)));
    std::cout << "landed on {" << env->quaffle->position.x << ", " << env->quaffle->position.y << "}" << std::endl;
}

TEST(shot_test, shot_on_goal){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot testShot(env, env->team1->chasers[2], env->quaffle, {14, 8});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 2);
    EXPECT_EQ(res.first[0], gameController::ShotResult::ThrowSuccess);
    EXPECT_EQ(res.first[1], gameController::ShotResult::ScoreLeft);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(14, 8));
}

TEST(shot_test, shot_on_goal1){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot testShot(env, env->team1->chasers[2], env->quaffle, {14, 4});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 2);
    EXPECT_EQ(res.first[0], gameController::ShotResult::ThrowSuccess);
    EXPECT_EQ(res.first[1], gameController::ShotResult::ScoreLeft);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(14, 4));
}

TEST(shot_test, invalid_shot_on_goal){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team2->chasers[0]->position;
    gameController::Shot testShot(env, env->team2->chasers[0], env->quaffle, {2, 8});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(2, 8));
}

TEST(shot_test, invalid_shot_on_goal1){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Shot testShot(env, env->team1->chasers[0], env->quaffle, {2, 8});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(2, 8));
}
//--------------------------Bludger shot check------------------------------------------------------------------------

TEST(shot_test, valid_bludger_shot_check){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->seeker->position);
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Success);
}

TEST(shot_test, invalid_bludger_shot_check_no_ball){
    auto env = setup::createEnv();
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->seeker->position);
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

TEST(shot_test, invalid_bludger_shot_check_oob){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

TEST(shot_test, invalid_bludger_shot_check_no_beater){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->chasers[1]->position;
    auto testShot = gameController::Shot(env, env->team2->chasers[1], env->bludgers[0], env->team1->seeker->position);
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

TEST(shot_test, invalid_bludger_shot_check_too_far){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], {9, 2});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

TEST(shot_test, invalid_bludger_shot_check_path_blocked){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], {5, 5});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Impossible);
}

//---------------------------Bludger shot execute-----------------------------------------------------------------------

TEST(shot_test, bludger_shot_on_empty_cell){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], {6, 2});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 0);
    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(6, 2));
}

TEST(shot_test, bludger_shot_on_Seeker){
    auto env = setup::createEnv({0, {}, {}, {0, 1, 0, 0, 0, 0}, {}});
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->seeker->position);
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::Knockout);
    EXPECT_NE(env->bludgers[0]->position, env->team1->seeker->position);
    EXPECT_TRUE(env->team1->seeker->knockedOut);
}

TEST(shot_test, bludger_shot_on_Chaser_with_ball){
    using P = gameModel::Position;
    auto env = setup::createEnv({0, {}, {}, {0, 1, 0, 0, 0, 0}, {}});
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    env->quaffle->position = env->team2->chasers[0]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team2->chasers[0]->position);
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ShotResult::Knockout);
    EXPECT_NE(env->bludgers[0]->position, env->team1->seeker->position);
    EXPECT_TRUE(env->team2->chasers[0]->knockedOut);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(7, 0), P(6, 0), P(5, 0), P(5, 1), P(5, 2), P(6, 2), P(7, 2), P(7, 1)));
}

TEST(shot_test, bludger_shot_on_Beater){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->beaters[1]->position);
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 0);
    EXPECT_EQ(env->bludgers[0]->position, env->team1->beaters[1]->position);
    EXPECT_FALSE(env->team1->beaters[1]->knockedOut);
}

//---------------------------getAllLandingCells----------------------------------------------------------------------
TEST(shot_Test, get_free_landing_cells1){
    auto env = setup::createEnv();
    auto testShot = gameController::Shot(env, env->team1.get()->seeker, env->quaffle, gameModel::Position{16,8});
    std::vector<gameModel::Position> testVector = {gameModel::Position(13,11), gameModel::Position(14,11),
                                                   gameModel::Position(13, 10), gameModel::Position(14, 10), gameModel::Position(15 ,10),
                                                   gameModel::Position(13, 9), gameModel::Position(14, 9), gameModel::Position(15, 9),
                                                   gameModel::Position(13, 8), gameModel::Position(14, 8), gameModel::Position(15, 8),
                                                   gameModel::Position(13, 7), gameModel::Position(14, 7), gameModel::Position(15, 7), gameModel::Position(16, 7),
                                                   gameModel::Position(13, 6), gameModel::Position(14, 6), gameModel::Position(15, 6), gameModel::Position(16, 6),
                                                   gameModel::Position(13, 5), gameModel::Position(14, 5), gameModel::Position(15, 5), gameModel::Position(16, 5)};
    EXPECT_EQ(testShot.getAllLandingCells(), testVector);
}

TEST(shot_Test, get_free_landing_cells2){
    auto env = setup::createEnv();
    env->team1.get()->seeker.position = {15 , 6}
    auto testShot = gameController::Shot(env, env->team1.get()->seeker, env->quaffle, gameModel::Position{16,8});
    std::vector<gameModel::Position> testVector = {gameModel::Position(13,11), gameModel::Position(14,11),
                                               gameModel::Position(13, 10), gameModel::Position(14, 10), gameModel::Position(15 ,10),
                                               gameModel::Position(13, 9), gameModel::Position(14, 9), gameModel::Position(15, 9),
                                               gameModel::Position(13, 8), gameModel::Position(14, 8), gameModel::Position(15, 8),
                                               gameModel::Position(13, 7), gameModel::Position(14, 7), gameModel::Position(15, 7), gameModel::Position(16, 7),
                                               gameModel::Position(13, 6), gameModel::Position(14, 6), gameModel::Position(16, 6),
                                               gameModel::Position(13, 5), gameModel::Position(14, 5), gameModel::Position(15, 5), gameModel::Position(16, 5)};
    EXPECT_EQ(testShot.getAllLandingCells(), testVector);
}

//---------------------------Move Foul Check tests----------------------------------------------------------------------

TEST(move_test, move_foul_ramming) {
    auto env = setup::createEnv();

    auto player = env->getPlayer({8, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->team1->keeper->position = gameModel::Position(8, 6);

    player = env->getPlayer({7, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->team2->keeper->position = gameModel::Position(7, 6);

    gameController::Move mv(env, env->team2->keeper, gameModel::Position(8, 6));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::Ramming);
}

TEST(move_test, move_foul_none) {
    auto env = setup::createEnv();

    auto player = env->getPlayer({8, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->team2->seeker->position = gameModel::Position(8, 6);

    player = env->getPlayer({7, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->team2->keeper->position = gameModel::Position(7, 6);

    env->snitch->position = gameModel::Position(5, 5);

    gameController::Move mv(env, env->team2->keeper, gameModel::Position(8, 6));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 0);
}

TEST(move_test, move_foul_blocksnitch) {
    auto env = setup::createEnv();
    env->snitch->exists = true;

    auto player = env->getPlayer({8, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->snitch->position = gameModel::Position(8, 6);

    player = env->getPlayer({7, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->team2->keeper->position = gameModel::Position(7, 6);

    gameController::Move mv(env, env->team2->keeper, gameModel::Position(8, 6));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::BlockSnitch);
}

TEST(move_test, move_foul_multipleoffence) {
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(16, 7);
    env->team1->chasers[1]->position = gameModel::Position(11, 7);

    auto player = env->getPlayer({12, 7});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team1->chasers[1], gameModel::Position(12, 7));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::MultipleOffence);
}

TEST(move_test, move_foul_blockgoal) {
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(1, 6);

    auto player = env->getPlayer({2, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(2, 6));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::BlockGoal);
}

TEST(move_test, move_foul_chargeGoal) {
    auto env = setup::createEnv();

    auto player = env->getPlayer({1, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }

    env->team2->chasers[0]->position = gameModel::Position(1, 6);
    env->quaffle->position = gameModel::Position(1, 6);
    env->snitch->position = gameModel::Position(5,5);

    player = env->getPlayer({2, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team2->chasers[0], gameModel::Position(2, 6));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::ChargeGoal);
}

//---------------------------Move Check tests---------------------------------------------------------------------------

TEST(move_test, move_check_impossible0) {
    auto env = setup::createEnv();

    env->team1->keeper->position = gameModel::Position(7, 1);

    gameController::Move mv(env, env->team1->keeper, gameModel::Position(7, 3));

    EXPECT_EQ(mv.check(), gameController::ActionCheckResult::Impossible);

}

TEST(move_test, move_check_impossible1) {
    auto env = setup::createEnv();

    env->team1->keeper->position = gameModel::Position(3, 0);

    gameController::Move mv(env, env->team1->keeper, gameModel::Position(2, 0));

    EXPECT_EQ(mv.check(), gameController::ActionCheckResult::Impossible);

}

TEST(move_test, move_check_success) {
    auto env = setup::createEnv();

    env->team1->keeper->position = gameModel::Position(7, 1);
    env->snitch->position = gameModel::Position(5,5);

    auto player = env->getPlayer({7, 2});
    if (player.has_value()) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team1->keeper, gameModel::Position(7, 2));

    EXPECT_EQ(mv.check(), gameController::ActionCheckResult::Success);
}

TEST(move_test, move_check_foul) {
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(16, 6);
    env->team1->chasers[1]->position = gameModel::Position(11, 6);

    gameController::Move mv(env, env->team1->chasers[1], gameModel::Position(12, 6));

    EXPECT_EQ(mv.check(), gameController::ActionCheckResult::Foul);
}

//---------------------------Move Execute Move--------------------------------------------------------------------------

// multiple offence
TEST(move_test, move_execute0) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    env->team1->chasers[0]->position = gameModel::Position(12, 6);
    env->team1->chasers[1]->position = gameModel::Position(11, 5);
    gameController::Move mv(env, env->team1->chasers[1], gameModel::Position(12, 5));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->chasers[1]->isFined);
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::MultipleOffence);

}

// charge goal
TEST(move_test, move_execute1) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    env->team1->chasers[0]->position = gameModel::Position(13, 6);
    env->quaffle->position = gameModel::Position(13, 6);
    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(14, 6));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->chasers[0]->isFined);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(14, 6));
    EXPECT_EQ(mvRes.first.size(), 1);
    EXPECT_EQ(mvRes.first[0], gameController::ShotResult::ScoreLeft);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::ChargeGoal);
}

// illegal move
TEST(move_test, move_execute2) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(14, 6));

    std::pair<std::vector<gameController::ShotResult>, std::vector<gameModel::Foul>> mvRes;

    EXPECT_ANY_THROW(mvRes = mv.execute());
    EXPECT_EQ(env->team1->chasers[0]->position, gameModel::Position(2, 10));
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 0);
}

// illegal move
TEST(move_test, move_execute3) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(0, 0));

    std::pair<std::vector<gameController::ShotResult>, std::vector<gameModel::Foul>> mvRes;

    EXPECT_ANY_THROW(mvRes = mv.execute());
    EXPECT_EQ(env->team1->chasers[0]->position, gameModel::Position(2, 10));
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 0);
}

// ramming
TEST(move_test, move_execute4) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    auto oldPos = env->team2->keeper->position;

    env->quaffle->position = oldPos;
    gameController::Move mv(env, env->team1->keeper, oldPos);

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->keeper->isFined);
    EXPECT_FALSE(env->team2->keeper->position == oldPos);
    EXPECT_FALSE(env->quaffle->position == oldPos);
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::Ramming);
}

// block goal
TEST(move_test, move_execute5) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    gameController::Move mv(env, env->team1->beaters[0], gameModel::Position(2, 4));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->beaters[0]->isFined);
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::BlockGoal);
}

// block snitch
TEST(move_test, move_execute6) {
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});

    env->snitch->exists = true;
    env->snitch->position = gameModel::Position(2, 3);

    gameController::Move mv(env, env->team1->beaters[0], gameModel::Position(2, 3));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->beaters[0]->isFined);
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::BlockSnitch);
}