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

TEST(teleport_test, execute0){
    auto env = setup::createEnv();
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    auto possibleCells = env->getAllFreeCells();

    EXPECT_EQ(testTeleport.execute(), gameController::ActionCheckResult::Success);

    bool success = false;
    for(const auto &pos : possibleCells){
       if(env->team1->seeker->position == pos){
           success = true;
           break;
       }
    }

    EXPECT_TRUE(success);
}

TEST(teleport_test, execute1){
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});
    gameController::Teleport testTeleport(env, env->team1, env->team1->seeker);
    auto possibleCells = env->getAllFreeCells();

    EXPECT_EQ(testTeleport.execute(), gameController::ActionCheckResult::Foul);

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

TEST(ranged_attack_test, execute_no_ball0){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    gameController::RangedAttack testAttack(env, env->team1, env->team2->seeker);

    EXPECT_EQ(testAttack.execute(), gameController::ActionCheckResult::Success);

    EXPECT_THAT(env->team2->seeker->position, testing::AnyOf(P(11, 7), P(10, 8), P(10, 9), P(11, 9), P(12, 9), P(12, 8), P(12, 7)));
}

TEST(ranged_attack_test, execute_no_ball1){
    using P = gameModel::Position;
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});
    gameController::RangedAttack testAttack(env, env->team1, env->team2->seeker);

    EXPECT_EQ(testAttack.execute(), gameController::ActionCheckResult::Foul);

    EXPECT_THAT(env->team2->seeker->position, testing::AnyOf(P(11, 7), P(10, 8), P(10, 9), P(11, 9), P(12, 9), P(12, 8), P(12, 7)));
}

TEST(ranged_attack_test, execute_target_with_quaffle0){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    env->quaffle->position = env->team2->keeper->position;
    gameController::RangedAttack testAttack(env, env->team1, env->team2->keeper);

    EXPECT_EQ(testAttack.execute(), gameController::ActionCheckResult::Success);

    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(13, 11), P(12, 12), P(14, 11)));
    EXPECT_THAT(env->team2->keeper->position, testing::AnyOf(P(13, 11), P(12, 12), P(14, 11)));
}

TEST(ranged_attack_test, execute_target_with_quaffle1){
    using P = gameModel::Position;
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});
    env->quaffle->position = env->team2->keeper->position;
    gameController::RangedAttack testAttack(env, env->team1, env->team2->keeper);

    EXPECT_EQ(testAttack.execute(), gameController::ActionCheckResult::Foul);

    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(13, 11), P(12, 12), P(14, 11)));
    EXPECT_THAT(env->team2->keeper->position, testing::AnyOf(P(13, 11), P(12, 12), P(14, 11)));
}

//----------------------------------------------Impulse-----------------------------------------------------------------

TEST(impulse_test, possible){
    auto env = setup::createEnv();
    gameController::Impulse testImpulse(env, env->team1);
    EXPECT_TRUE(testImpulse.isPossible());
}

TEST(impulse_test, no_uses_left){
    auto env = setup::createEnv();
    gameController::Impulse testImpulse(env, env->team1);
    while(env->team1->fanblock.getUses(testImpulse.getType())){
        env->team1->fanblock.banFan(testImpulse.getType());
    }

    EXPECT_FALSE(testImpulse.isPossible());
}

TEST(impulse_test, execute0){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Impulse testImpulse(env, env->team1);

    EXPECT_EQ(testImpulse.execute(), gameController::ActionCheckResult::Success);

    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(2, 9), P(1, 9), P(1, 10), P(2, 11), P(3, 11), P(3, 10), P(3, 9)));
}

TEST(impulse_test, execute1){
    using P = gameModel::Position;
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});
    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Impulse testImpulse(env, env->team1);

    EXPECT_EQ(testImpulse.execute(), gameController::ActionCheckResult::Foul);

    EXPECT_THAT(env->quaffle->position, testing::AnyOf(P(2, 9), P(1, 9), P(1, 10), P(2, 11), P(3, 11), P(3, 10), P(3, 9)));
}

//----------------------------------------------SnitchPush--------------------------------------------------------------

TEST(snitch_push_test, possible){
    auto env = setup::createEnv();
    gameController::SnitchPush testSnitchPush(env, env->team1);
    EXPECT_TRUE(testSnitchPush.isPossible());
}

TEST(snitch_push_test, no_uses_left){
    auto env = setup::createEnv();
    gameController::Impulse testImpuls(env, env->team1);
    while(env->team1->fanblock.getUses(testImpuls.getType())){
        env->team1->fanblock.banFan(testImpuls.getType());
    }

    EXPECT_FALSE(testImpuls.isPossible());
}

TEST(snitch_push_test, execute0){
    using P = gameModel::Position;
    auto env = setup::createEnv();
    env->snitch->exists = true;
    env->snitch->position = {10, 8};
    gameController::SnitchPush testSnitchPush(env, env->team1);

    EXPECT_EQ(testSnitchPush.execute(), gameController::ActionCheckResult::Success);

    EXPECT_THAT(env->snitch->position, testing::AnyOf(P(11, 7), P(9, 7), P(9, 8), P(10, 9), P(11, 9)));
}

TEST(snitch_push_test, execute1){
    using P = gameModel::Position;
    auto env = setup::createEnv({0, {}, {1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1, 1}, {}});
    env->snitch->exists = true;
    env->snitch->position = {10, 8};
    gameController::SnitchPush testSnitchPush(env, env->team1);

    EXPECT_EQ(testSnitchPush.execute(), gameController::ActionCheckResult::Foul);

    EXPECT_THAT(env->snitch->position, testing::AnyOf(P(11, 7), P(9, 7), P(9, 8), P(10, 9), P(11, 9)));
}

//----------------------------------------------FanToInterfernce/ InterferenceToFan--------------------------------------------------------------
TEST(fan_test, fan_to_interference){
    EXPECT_EQ(gameModel::Fanblock::fanToInterference(communication::messages::types::FanType::GOBLIN), gameModel::InterferenceType::RangedAttack);
    EXPECT_EQ(gameModel::Fanblock::fanToInterference(communication::messages::types::FanType::TROLL), gameModel::InterferenceType::Impulse);
    EXPECT_EQ(gameModel::Fanblock::fanToInterference(communication::messages::types::FanType::ELF), gameModel::InterferenceType::Teleport);
    EXPECT_EQ(gameModel::Fanblock::fanToInterference(communication::messages::types::FanType::NIFFLER), gameModel::InterferenceType::SnitchPush);
}

TEST(fan_test, interference_to_fan) {
    EXPECT_EQ(gameModel::Fanblock::interferenceToFan(gameModel::InterferenceType::RangedAttack), communication::messages::types::FanType::GOBLIN);
    EXPECT_EQ(gameModel::Fanblock::interferenceToFan(gameModel::InterferenceType::Teleport), communication::messages::types::FanType::ELF);
    EXPECT_EQ(gameModel::Fanblock::interferenceToFan(gameModel::InterferenceType::Impulse), communication::messages::types::FanType::TROLL);
    EXPECT_EQ(gameModel::Fanblock::interferenceToFan(gameModel::InterferenceType::SnitchPush), communication::messages::types::FanType::NIFFLER);
}