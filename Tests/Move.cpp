//
// Created by tim on 11.06.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Interference.h>
#include "Action.h"
#include "setup.h"

//---------------------------Move Foul Check tests----------------------------------------------------------------------

TEST(move_test, move_foul_ramming) {
    auto env = setup::createEnv();

    auto player = env->getPlayer({8, 6});
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->team1->keeper->position = gameModel::Position(8, 6);

    player = env->getPlayer({7, 6});
    if (player.has_value() && !player.value()->isFined) {
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
    if (player.has_value() && !player.value()->isFined) {
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
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }
    env->snitch->position = gameModel::Position(8, 6);

    player = env->getPlayer({7, 6});
    if (player.has_value() && !player.value()->isFined) {
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
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team1->chasers[1], gameModel::Position(12, 7));

    std::vector<gameModel::Foul> fouls = mv.checkForFoul();

    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::MultipleOffence);
}

TEST(move_test, move_foul_multipleoffence_banned){
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(16, 7);
    env->team1->chasers[0]->isFined = true;
    env->team1->chasers[1]->position = gameModel::Position(11, 7);

    auto player = env->getPlayer({12, 7});
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team1->chasers[1], gameModel::Position(12, 7));
    std::vector<gameModel::Foul> fouls = mv.checkForFoul();
    EXPECT_TRUE(fouls.empty());
}

TEST(move_test, move_foul_multipleoffence_check_only_once){
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(16, 7);
    env->team1->chasers[1]->position = gameModel::Position(11, 7);

    auto player = env->getPlayer({12, 7});
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }

    gameController::Move mv(env, env->team1->chasers[1], gameModel::Position(12, 7));
    std::vector<gameModel::Foul> fouls = mv.checkForFoul();
    mv.execute();
    EXPECT_EQ(fouls.size(), 1);
    EXPECT_EQ(fouls[0], gameModel::Foul::MultipleOffence);
    mv = gameController::Move(env, env->team1->chasers[1], gameModel::Position(13, 7));
    fouls = mv.checkForFoul();
    EXPECT_TRUE(fouls.empty());
}

TEST(move_test, move_foul_blockgoal) {
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(1, 6);

    auto player = env->getPlayer({2, 6});
    if (player.has_value() && !player.value()->isFined) {
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
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }

    env->team2->chasers[0]->position = gameModel::Position(1, 6);
    env->quaffle->position = gameModel::Position(1, 6);
    env->snitch->position = gameModel::Position(5,5);

    player = env->getPlayer({2, 6});
    if (player.has_value() && !player.value()->isFined) {
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
    if (player.has_value() && !player.value()->isFined) {
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
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

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
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->team1->chasers[0]->position = gameModel::Position(13, 6);
    env->quaffle->position = gameModel::Position(13, 6);
    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(14, 6));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->chasers[0]->isFined);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(14, 6));
    EXPECT_EQ(mvRes.first.size(), 1);
    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::ScoreLeft);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::ChargeGoal);
}

// illegal move
TEST(move_test, move_execute2) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(14, 6));

    std::pair<std::vector<gameController::ActionResult>, std::vector<gameModel::Foul>> mvRes;

    EXPECT_ANY_THROW(mvRes = mv.execute());
    EXPECT_EQ(env->team1->chasers[0]->position, gameModel::Position(2, 10));
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 0);
}

// illegal move
TEST(move_test, move_execute3) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    gameController::Move mv(env, env->team1->chasers[0], gameModel::Position(0, 0));

    std::pair<std::vector<gameController::ActionResult>, std::vector<gameModel::Foul>> mvRes;

    EXPECT_ANY_THROW(mvRes = mv.execute());
    EXPECT_EQ(env->team1->chasers[0]->position, gameModel::Position(2, 10));
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 0);
}

// ramming
TEST(move_test, move_execute4) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    auto oldPos = env->team2->keeper->position;

    env->quaffle->position = oldPos;
    gameController::Move mv(env, env->team1->keeper, oldPos);

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->keeper->isFined);
    EXPECT_FALSE(env->team2->keeper->position == oldPos);
    EXPECT_FALSE(env->quaffle->position == oldPos);
    EXPECT_EQ(mvRes.first.size(), 1);
    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::FoolAway);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::Ramming);
}

// block goal
TEST(move_test, move_execute5) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    gameController::Move mv(env, env->team1->beaters[0], gameModel::Position(2, 4));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->beaters[0]->isFined);
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::BlockGoal);
}

// block snitch
TEST(move_test, move_execute6) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->snitch->exists = true;
    env->snitch->position = gameModel::Position(2, 3);

    gameController::Move mv(env, env->team1->beaters[0], gameModel::Position(2, 3));

    auto mvRes = mv.execute();

    EXPECT_TRUE(env->team1->beaters[0]->isFined);
    EXPECT_EQ(mvRes.first.size(), 0);
    EXPECT_EQ(mvRes.second.size(), 1);
    EXPECT_EQ(mvRes.second[0], gameModel::Foul::BlockSnitch);
}

//Catch Snitch
TEST(move_test, move_execute7){
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});
    env->snitch->exists = true;
    env->snitch->position = {3, 4};
    env->team1->seeker->position = {3, 4};

    gameController::Move mv(env, env->team1->seeker, gameModel::Position(3,4));
    auto mvRes = mv.execute();

    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::SnitchCatch);
}

//Fail to catch the Snitch because Snitch doesn't exist
TEST(move_test, move_execute8){
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});
    env->snitch->exists = false;
    env->snitch->position = {3, 4};
    env->team1->seeker->position = {3, 4};

    gameController::Move mv(env, env->team1->seeker, gameModel::Position(3,4));
    auto mvRes = mv.execute();

    EXPECT_TRUE(mvRes.first.empty());
}

//Fail to catch the Snitch because Snitch hasn't the same Position like the Seeker
TEST(move_test, move_execute9){
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});
    env->snitch->exists = true;
    env->snitch->position = {3, 4};
    env->team1->seeker->position = {3, 4};

    gameController::Move mv(env, env->team1->seeker, gameModel::Position(4,4));
    auto mvRes = mv.execute();

    EXPECT_TRUE(mvRes.first.empty());
}

//Fail to catch the Snitch because there is no Seeker on the same field
TEST(move_test, move_execute10){
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});
    env->snitch->exists = false;
    env->snitch->position = {3, 4};
    env->team1->keeper->position = {3, 4};

    gameController::Move mv(env, env->team1->keeper, gameModel::Position(3,4));
    auto mvRes = mv.execute();

    EXPECT_TRUE(mvRes.first.empty());
}

TEST(move_test, move_execute_all){
    auto env = setup::createEnv();
    gameController::Move move(env, env->team2->seeker, gameModel::Position{12, 9});
    auto resList = move.executeAll();
    EXPECT_EQ(resList.size(), 1);
    EXPECT_DOUBLE_EQ(resList[0].second, 1);
    EXPECT_EQ(resList[0].first->team2->seeker->position, gameModel::Position(12, 9));
    EXPECT_EQ(env->team2->seeker->position, gameModel::Position(11, 8));
}

TEST(move_test, move_execute_all_snitch_catch){
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 0.8, 1, 1}, {}});
    env->snitch->position = {12, 9};
    env->snitch->exists = true;
    gameController::Move move(env, env->team2->seeker, gameModel::Position{12, 9});
    auto resList = move.executeAll();
    EXPECT_EQ(resList.size(), 2);
    auto catchProb = env->config.gameDynamicsProbs.catchSnitch;
    EXPECT_DOUBLE_EQ(resList[0].second, catchProb);
    EXPECT_EQ(resList[0].first->team2->score, gameController::SNITCH_POINTS);
    EXPECT_EQ(resList[0].first->team2->seeker->position, gameModel::Position(12, 9));
    EXPECT_DOUBLE_EQ(resList[1].second, 1 - catchProb);
    EXPECT_EQ(resList[1].first->team2->score, 0);
    EXPECT_EQ(resList[1].first->team2->seeker->position, gameModel::Position(12, 9));
    EXPECT_EQ(env->team2->seeker->position, gameModel::Position(11, 8));
    EXPECT_EQ(env->team2->score, 0);
}

TEST(move_test, move_execute_all_charge_goal){
    auto env = setup::createEnv({0, {1, 0.4, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 0.8, 1, 1}, {}});
    gameModel::Position target(14, 8);
    env->team1->chasers[0]->position = {13, 8};
    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Move move(env, env->team1->chasers[0], target);
    auto resList = move.executeAll();
    EXPECT_EQ(resList.size(), 2);
    EXPECT_EQ(resList[0].first->quaffle->position, target);
    EXPECT_EQ(resList[0].first->team1->chasers[0]->position, target);
    EXPECT_EQ(resList[0].first->team1->score, gameController::GOAL_POINTS);
    EXPECT_FALSE(resList[0].first->team1->chasers[0]->isFined);
    EXPECT_DOUBLE_EQ(resList[0].second, 1 - env->config.foulDetectionProbs.chargeGoal);
    EXPECT_EQ(resList[1].first->quaffle->position, target);
    EXPECT_EQ(resList[1].first->team1->chasers[0]->position, target);
    EXPECT_EQ(resList[1].first->team1->score, gameController::GOAL_POINTS);
    EXPECT_DOUBLE_EQ(resList[1].second, env->config.foulDetectionProbs.chargeGoal);
    EXPECT_TRUE(resList[1].first->team1->chasers[0]->isFined);
    EXPECT_EQ(env->team1->chasers[0]->position, gameModel::Position(13, 8));
    EXPECT_EQ(env->quaffle->position, gameModel::Position(13, 8));
}

TEST(move_test, move_execute_all_ramming){
    auto env = setup::createEnv({0, {1, 0.4, 1, 0.5, 1, 1, 1, 1, 1, 1}, {1, 1, 0.8, 1, 1}, {}});
    auto target = env->team1->chasers[2]->position;
    gameController::Move move(env, env->team2->seeker, target);
    auto resList = move.executeAll();
    EXPECT_EQ(resList.size(), 16);

    double sum = 0;
    std::deque<gameModel::Position> poses = {gameModel::Position(9, 6), gameModel::Position(10, 6), gameModel::Position(11, 6),
                                             gameModel::Position(9, 7), gameModel::Position(11, 7), gameModel::Position(9, 8),
                                             gameModel::Position(10, 8), gameModel::Position(11, 8),

                                             gameModel::Position(9, 6), gameModel::Position(10, 6), gameModel::Position(11, 6),
                                             gameModel::Position(9, 7), gameModel::Position(11, 7), gameModel::Position(9, 8),
                                             gameModel::Position(10, 8), gameModel::Position(11, 8)};
    for(const auto &res : resList) {
        sum += res.second;
        EXPECT_EQ(res.first->team2->seeker->position, target);
        for(auto p = poses.begin(); p < poses.end();) {
            if(*p == res.first->team1->chasers[2]->position) {
                p = poses.erase(p);
                break;
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_DOUBLE_EQ(sum, 1);
    EXPECT_EQ(env->team2->seeker->position, gameModel::Position(11, 8));
    EXPECT_EQ(env->team1->chasers[2]->position, target);
}

TEST(move_test, move_execute_all_ramming_with_quaffle){
    auto env = setup::createEnv({0, {1, 0.4, 1, 0.5, 1, 1, 1, 1, 1, 1}, {1, 1, 0.8, 1, 1}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Move move(env, env->team2->seeker, env->team1->chasers[2]->position);
    auto resList = move.executeAll();

    double sum = 0;
    EXPECT_EQ(resList.size(), 7 * 8 * 2);
    for(const auto &res : resList){
        sum += res.second;
    }

    EXPECT_NEAR(sum, 1, 0.000001);
    EXPECT_EQ(env->team2->seeker->position, gameModel::Position(11, 8));
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
    EXPECT_EQ(env->team1->chasers[2]->position, gameModel::Position(10, 7));
}

TEST(move_test, move_execute_all_charge_goal_with_defender){
    auto env = setup::createEnv({0, {1, 0.4, 1, 0.5, 1, 1, 1, 1, 1, 1}, {1, 1, 0.8, 1, 1}, {}});
    gameModel::Position target(14, 8);
    env->team1->chasers[2]->position = {13, 8};
    env->team2->keeper->position = target;
    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Move move(env, env->team1->chasers[2], target);
    auto resList = move.executeAll();

    double sum = 0;
    EXPECT_EQ(resList.size(), 8 * 2);
    for(const auto &res : resList){
        sum += res.second;
    }

    EXPECT_GE(sum, 0.999999);
    EXPECT_LE(sum, 1.000001);
    EXPECT_EQ(env->team1->chasers[2]->position, gameModel::Position(13, 8));
    EXPECT_EQ(env->team2->keeper->position, target);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
}
