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

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Valid Keeper throw_check
TEST(shot_test, valid_keeper_throw_check) {
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}

//Invalid Keeper throw_check (Ball not at keeper position)
TEST(shot_test, inv_keeper_throw_check){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.chasers[0]->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Valid Chaser throw_check
TEST(shot_test, valid_chaser_throw_check){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.chasers[0]->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.chasers[0], env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}

//Invalid Seeker throw_check
TEST(shot_test, inv_seeker_throw_check){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.seeker->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.seeker, env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Invalid Beater throw_check
TEST(shot_test, inv_beater_throw_check){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.beaters[0]->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.beaters[0], env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//-----------------Throw execute----------------------------------------------------------------------------------------

//Keeper throw_checks ball to centre, no intercept, 100% chance of success
TEST(shot_test, success_throw_execute){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {8, 6});
    testShot.execute();
    EXPECT_EQ(env.quaffle->position, gameModel::Position(8, 6));
}

//Keeper throw_checks ball to centre, intercepted
TEST(shot_test, success_throw_execute_intercept){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 1, 0}, {}});

    env.quaffle->position = env.team1.keeper->position;
env.team2.chasers[1]->position = {9, 7};
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {8, 6});
    testShot.execute();
    EXPECT_EQ(env.quaffle->position, gameModel::Position(9, 7));
}

TEST(shot_test, success_throw_execute_intercept_bounce_off){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 1, 0}, {}});

    env.quaffle->position = env.team1.keeper->position;
    env.team2.beaters[0]->position = {10, 9};
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {8, 6});
    testShot.execute();
    EXPECT_THAT(env.quaffle->position, testing::AnyOf(gameModel::Position(10, 8), gameModel::Position(9, 8),
            gameModel::Position(9, 10), gameModel::Position(10, 10), gameModel::Position(11, 10), gameModel::Position(11, 9)));
}

TEST(shot_test, success_throw_execute_fail_and_disperse){
    auto env = setup::createEnv({0, {}, {}, {0, 0, 0, 0, 0, 0}, {}});

    env.team1.keeper->position = {0, 8};
    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {16, 8});
    testShot.execute();
    EXPECT_THAT(env.quaffle->position, testing::AnyOf(gameModel::Position(16, 7), gameModel::Position(15, 7),
                                                      gameModel::Position(15, 8), gameModel::Position(15, 9)));
}

//--------------------------Bludger shot check------------------------------------------------------------------------

TEST(shot_test, valid_bludger_shot_check){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], env->team1.seeker->position);
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}


TEST(shot_test, invalid_bludger_shot_check_no_ball){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], env->team1.seeker->position);
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}


TEST(shot_test, invalid_bludger_shot_check_oob){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}


TEST(shot_test, invalid_bludger_shot_check_no_beater){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.chasers[1]->position;
    auto testShot = gameController::Shot(env, env->team2.chasers[1], env->bludgers[0], env->team1.seeker->position);
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}


TEST(shot_test, invalid_bludger_shot_check_too_far){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], {9, 2});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

TEST(shot_test, invalid_bludger_shot_check_path_blocked){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], {5, 5});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//---------------------------Bludger shot execute-----------------------------------------------------------------------

TEST(shot_test, bludger_shot_on_empty_cell){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], {6, 2});
    testShot.execute();
    EXPECT_EQ(env->bludgers[0]->position, gameModel::Position(6, 2));
}

TEST(shot_test, bludger_shot_on_Seeker){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv({0, {}, {}, {0, 1, 0, 0, 0, 0}, {}}));
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], env->team1.seeker->position);
    testShot.execute();
    EXPECT_NE(env->bludgers[0]->position, env->team1.seeker->position);
    EXPECT_TRUE(env->team1.seeker->knockedOut);
}


TEST(shot_test, bludger_shot_on_Beater){
    auto env = std::make_shared<gameModel::Environment>(setup::createEnv());
    env->bludgers[0]->position = env->team2.beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2.beaters[1], env->bludgers[0], env->team1.beaters[1]->position);
    testShot.execute();
    EXPECT_EQ(env->bludgers[0]->position, env->team1.beaters[1]->position);
    EXPECT_FALSE(env->team1.beaters[1]->knockedOut);
}

//---------------------------Move tests---------------------------------------------------------------------------------

TEST(move_test, move_foul_ramming) {
    auto env = setup::createEnv();

    auto player = env.getPlayer({8, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }
    env.team1.keeper->position = gameModel::Position(8, 6);

    player = env.getPlayer({7, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }
    env.team2.keeper->position = gameModel::Position(7, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team2.keeper, gameModel::Position(8, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::Ramming);
}

TEST(move_test, move_foul_none) {
    auto env = setup::createEnv();

    auto player = env.getPlayer({8, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }
    env.team2.seeker->position = gameModel::Position(8, 6);

    player = env.getPlayer({7, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }
    env.team2.keeper->position = gameModel::Position(7, 6);

    env.snitch->position = gameModel::Position(5, 5);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team2.keeper, gameModel::Position(8, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::None);
}

TEST(move_test, move_foul_blocksnitch) {
    auto env = setup::createEnv();
    env.snitch->exists = true;

    auto player = env.getPlayer({8, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }
    env.snitch->position = gameModel::Position(8, 6);

    player = env.getPlayer({7, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }
    env.team2.keeper->position = gameModel::Position(7, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team2.keeper, gameModel::Position(8, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::BlockSnitch);
}

TEST(move_test, move_foul_multipleoffence) {
    auto env = setup::createEnv();

    env.team1.chasers[0]->position = gameModel::Position(16, 7);
    env.team1.chasers[1]->position = gameModel::Position(11, 7);

    auto player = env.getPlayer({12, 7});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.chasers[1], gameModel::Position(12, 7));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::MultipleOffence);
}

TEST(move_test, move_foul_blockgoal) {
    auto env = setup::createEnv();

    env.team1.chasers[0]->position = gameModel::Position(1, 6);

    auto player = env.getPlayer({2, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.chasers[0], gameModel::Position(2, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::BlockGoal);
}

//@TODO failt noch manchmal
TEST(move_test, move_foul_chargeGoal) {
    auto env = setup::createEnv();

    auto player = env.getPlayer({1, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    env.team2.chasers[0]->position = gameModel::Position(1, 6);
    env.quaffle->position = gameModel::Position(1, 6);
    env.snitch->position = gameModel::Position(5,5);

    player = env.getPlayer({2, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team2.chasers[0], gameModel::Position(2, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::ChargeGoal);
}

TEST(move_test, move_check_impossible0) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(7, 1);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.keeper, gameModel::Position(7, 3));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Impossible);

}

TEST(move_test, move_check_impossible1) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(3, 0);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.keeper, gameModel::Position(2, 0));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Impossible);

}

TEST(move_test, move_check_success) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(7, 1);
    env.snitch->position = gameModel::Position(5,5);

    auto player = env.getPlayer({7, 2});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.keeper, gameModel::Position(7, 2));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Success);
}

TEST(move_test, move_check_foul) {
    auto env = setup::createEnv();

    env.team1.chasers[0]->position = gameModel::Position(16, 6);
    env.team1.chasers[1]->position = gameModel::Position(11, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.chasers[1], gameModel::Position(12, 6));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Foul);
}
