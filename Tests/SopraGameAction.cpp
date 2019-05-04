//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "Action.h"
#include "setup.h"

//OOB throw
TEST(shot_test, shot_check1) {
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Valid Keeper throw
TEST(shot_test, shot_check2) {
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}

//Invalid Keeper throw (Ball not at keeper position)
TEST(shot_test, shot_check3){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.chasers[0]->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Valid Chaser throw
TEST(shot_test, shot_check4){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.chasers[0]->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.chasers[0], env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}

//Invalid Seeker throw
TEST(shot_test, shot_check5){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.seeker->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.seeker, env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Invalid Beater throw
TEST(shot_test, shot_check6){
    auto env = setup::createEnv();

    env.quaffle->position = env.team1.beaters[0]->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.beaters[0], env.quaffle, {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}


//Keeper throws ball to centre, no intercept, 100% chance of success
TEST(shot_test, shot_execute1){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env.quaffle->position = env.team1.keeper->position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env), env.team1.keeper, env.quaffle, {8, 6});
    testShot.execute();
    EXPECT_EQ(env.quaffle->position, gameModel::Position(8, 6));
}
