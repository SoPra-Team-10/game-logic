//
// Created by jonas on 30.04.19.
//

#include <gtest/gtest.h>
#include "Action.h"
#include "setup.h"

TEST(shot_test, shot_check1) {
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.keeper.position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
            std::make_shared<gameModel::Keeper>(env.team1.keeper), std::make_shared<gameModel::Quaffle>(env.quaffle), {0, 0});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Impossible);
}

TEST(shot_test, shot_check2) {
    auto env = setup::createEnv();

    env.quaffle.position = env.team1.keeper.position;
    gameController::Shot testShot(std::make_shared<gameModel::Environment>(env),
                                  std::make_shared<gameModel::Keeper>(env.team1.keeper), std::make_shared<gameModel::Quaffle>(env.quaffle), {12, 12});
    EXPECT_EQ(testShot.check(), gameController::ActionResult::Success);
}
