//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "Action.h"
#include "setup.h"

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

//Invalid Keeper throw_check (Ball not at keeper->position)
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


//Keeper throw_checks ball to centre, no intercept, 100% chance of success
TEST(shot_test, success_throw_execute){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {8, 6});
    testShot.execute();
    EXPECT_EQ(env.quaffle->position, gameModel::Position(8, 6));
}

//---------------------------Move tests---------------------------------------------------------------------------------

TEST(move_test, move_foul0) {
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

TEST(move_test, move_foul1) {
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

TEST(move_test, move_foul2) {
    auto env = setup::createEnv();

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

TEST(move_test, move_foul3) {
    auto env = setup::createEnv();

    env.team1.chasers[0]->position = gameModel::Position(16, 7);
    env.team1.chasers[1]->position = gameModel::Position(11, 7);

    auto player = env.getPlayer({12, 7});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.chasers[1], gameModel::Position(12, 7));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::MultibleOffence);
}

TEST(move_test, move_foul4) {
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

TEST(move_test, move_foul5) {
    auto env = setup::createEnv();

    env.team2.chasers[0]->position = gameModel::Position(1, 6);
    env.quaffle->position = gameModel::Position(1, 6);

    auto player = env.getPlayer({2, 6});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team2.chasers[0], gameModel::Position(2, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::ChargeGoal);
}

TEST(move_test, move_check0) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(7, 1);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.keeper, gameModel::Position(7, 3));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Impossible);

}

TEST(move_test, move_check1) {
    auto env = setup::createEnv();

    env.team1.keeper->position = gameModel::Position(7, 1);

    auto player = env.getPlayer({7, 2});
    if (player.has_value()) {
        gameController::moveToAdjacent(*player.value(), env);
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.keeper, gameModel::Position(7, 2));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Success);
}

TEST(move_test, move_check2) {
    auto env = setup::createEnv();

    env.team1.chasers[0]->position = gameModel::Position(16, 6);
    env.team1.chasers[1]->position = gameModel::Position(11, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            env.team1.chasers[1], gameModel::Position(12, 6));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Foul);
}
