//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "Action.h"
#include "setup.h"

//OOB throw_check
TEST(shot_test, oob_throw_check) {
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.keeper.position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
            std::make_shared<gameModel::Keeper>(env.team1.keeper), std::make_shared<gameModel::Quaffle>(env.quaffle), {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Valid Keeper throw_check
TEST(shot_test, valid_keeper_throw_check) {
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.keeper.position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Keeper>(env.team1.keeper), std::make_shared<gameModel::Quaffle>(env.quaffle), {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}

//Invalid Keeper throw_check (Ball not at keeper position)
TEST(shot_test, inv_keeper_throw_check){
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.chasers[0].position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Keeper>(env.team1.keeper), std::make_shared<gameModel::Quaffle>(env.quaffle), {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Valid Chaser throw_check
TEST(shot_test, valid_chaser_throw_check){
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.chasers[0].position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Chaser>(env.team1.chasers[0]), std::make_shared<gameModel::Quaffle>(env.quaffle), {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}

//Invalid Seeker throw_check
TEST(shot_test, inv_seeker_throw_check){
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.seeker.position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Seeker>(env.team1.seeker), std::make_shared<gameModel::Quaffle>(env.quaffle), {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

//Invalid Beater throw_check
TEST(shot_test, inv_beater_throw_check){
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.beaters[0].position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Beater>(env.team1.beaters[0]), std::make_shared<gameModel::Quaffle>(env.quaffle), {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}


//Keeper throw_checks ball to centre, no intercept, 100% chance of success
TEST(shot_test, success_throw_execute){
    auto env = setup::createEnv({0, {}, {}, {1, 0, 0, 0, 0, 0}, {}});

    env.quaffle.position = env.team1.keeper.position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Keeper>(env.team1.keeper), std::make_shared<gameModel::Quaffle>(env.quaffle), {8, 6});
    testShot.execute();
    EXPECT_EQ(env.quaffle.position, gameModel::Position(8, 6));
}
