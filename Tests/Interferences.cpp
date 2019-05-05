//
// Created by timluchterhand on 05.05.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "GameModel.h"
#include "GameController.h"
#include "setup.h"
#include "Interference.h"


TEST(teleport_test, possible){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    EXPECT_TRUE(testTeleport.isPossible());
}

TEST(teleport_test, no_teleports_left){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    while(env->team1->fanblock.getUses(testTeleport.getType()))
    env->team1->fanblock.banFan(testTeleport.getType());
    EXPECT_FALSE(testTeleport.isPossible());
}

TEST(teleport_test, execute_Seeker){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    auto possibleCells = env->getAllFreeCells();
    testTeleport.execute();
    bool success = false;
    for(const auto &pos : possibleCells){
       if(env->team1->seeker->position == pos){
           success = true;
       }
    }

    if(success){
        SUCCEED();
    } else {
        FAIL();
    }
}
