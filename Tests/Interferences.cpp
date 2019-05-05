//
// Created by timluchterhand on 05.05.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include "GameModel.h"
#include "GameController.h"
#include "setup.h"
#include "Interference.h"

//----------------------------Teleport----------------------------------------------------------------------------------
TEST(teleport_test, possible){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    EXPECT_TRUE(testTeleport.isPossible());
}

TEST(teleport_test, no_teleports_left){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    while(env->team1->fanblock.getUses(testTeleport.getType())){
        env->team1->fanblock.banFan(testTeleport.getType());
    }

    EXPECT_FALSE(testTeleport.isPossible());
}

TEST(teleport_test, execute){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    auto possibleCells = env->getAllFreeCells();
    testTeleport.execute();
    bool success = false;
    for(const auto &pos : possibleCells){
       if(env->team1->seeker->position == pos){
           success = true;
           break;
       }
    }

    EXPECT_TRUE(success);
}

//--------------------------RangedAttack--------------------------------------------------------------------------------
TEST(ranged_attack_test, possible){
    auto env = setup::createEnv();
    gameController::RangedAttack testAttack(env, env->team1, env->team2->seeker);
    EXPECT_TRUE(testAttack.isPossible());
}

TEST(ranged_attack_test, no_uses_left){
    auto env = setup::createEnv();
    gameController::RangedAttack testAttack(env, env->team1, env->team2->seeker);
    while(env->team1->fanblock.getUses(testAttack.getType())){
        env->team1->fanblock.banFan(testAttack.getType());
    }

    EXPECT_FALSE(testAttack.isPossible());
}

TEST(ranged_attack_test, own_player){
    auto env = setup::createEnv();
    gameController::RangedAttack testAttack(env, env->team1, env->team1->seeker);
    EXPECT_FALSE(testAttack.isPossible());
}

TEST(ranged_attack_test, execute_no_ball){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    gameController::RangedAttack testAttack(env, env->team1, env->team2->seeker);
    testAttack.execute();

    EXPECT_THAT(env->team2->seeker->position, testing::AnyOf(P(11, 7), P(10, 8), P(10, 9), P(11, 9), P(12, 9), P(12, 8), P(12, 7)));
}

TEST(ranged_attack_test, execute_target_with_quaffle){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    env->quaffle->position = env->team2->keeper->position;
    gameController::RangedAttack testAttack(env, env->team1, env->team2->keeper);
    testAttack.execute();
    std::cout << "Position Quaffle {" << env->quaffle->position.x << ", " << env->quaffle->position.y << "}" << std::endl;
    std::cout << "Position Keeper{" << env->team2->keeper->position.x << ", " << env->team2->keeper->position.y << "}" << std::endl;
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(13, 11), P(12, 12), P(14, 11)));
    EXPECT_THAT(env->team2->keeper->position, testing::AnyOf(P(13, 11), P(12, 12), P(14, 11)));
}

//----------------------------------------------Impulse-----------------------------------------------------------------

TEST(impulse_test, possible){
    auto env = setup::createEnv();
    gameController::Impulse testImpuls(env, env->team1);
    EXPECT_TRUE(testImpuls.isPossible());
}

TEST(impulse_test, no_uses_left){
    auto env = setup::createEnv();
    gameController::Impulse testImpuls(env, env->team1);
    while(env->team1->fanblock.getUses(testImpuls.getType())){
        env->team1->fanblock.banFan(testImpuls.getType());
    }

    EXPECT_FALSE(testImpuls.isPossible());
}


TEST(impulse_test, execute){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Impulse testImpuls(env, env->team1);
    testImpuls.execute();
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(2, 9), P(1, 9), P(1, 10), P(2, 11), P(3, 11), P(3, 10), P(3, 9)));
}
