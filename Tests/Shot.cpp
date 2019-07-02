//
// Created by tim on 11.06.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Interference.h>
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
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(8, 6));
}

//Keeper throw_checks ball to centre, intercepted
TEST(shot_test, throw_execute_intercept){
    auto env = setup::createEnv({0, {}, {1, 0, 0,  1, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[1]->position = {9, 7};
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::Intercepted);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(9, 7));
}

//Keeper throw_checks ball to centre, not intercepted since banned or fined
TEST(shot_test, throw_execute_no_intercept){
    auto env = setup::createEnv({0, {}, {1, 0, 0,  1, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[1]->position = {9, 7};
    env->team2->chasers[2]->position = {10, 8};
    env->team2->chasers[1]->isFined = true;
    env->team2->chasers[2]->knockedOut = true;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(8, 6));
}

TEST(shot_test, throw_execute_intercept_bounce_off){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 1, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    env->team2->beaters[0]->position = {10, 9};
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::Intercepted);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(10, 8), gameModel::Position(9, 8),
                                                       gameModel::Position(9, 10), gameModel::Position(10, 10), gameModel::Position(11, 10), gameModel::Position(11, 9)));
}

TEST(shot_test, throw_execute_fail_and_disperse){
    auto env = setup::createEnv({0, {}, {0, 0, 0, 0, 0}, {}});

    env->team1->keeper->position = {0, 8};
    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {16, 8});
    auto res = testShot.execute();
    EXPECT_GE(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::Miss);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(13,11), gameModel::Position(14,11),
                                                       gameModel::Position(13, 10), gameModel::Position(14, 10), gameModel::Position(15 ,10),
                                                       gameModel::Position(13, 9), gameModel::Position(14, 9), gameModel::Position(15, 9),
                                                       gameModel::Position(13, 8), gameModel::Position(14, 8), gameModel::Position(15, 8), gameModel::Position(16, 7),
                                                       gameModel::Position(13, 7), gameModel::Position(14, 7), gameModel::Position(15, 7), gameModel::Position(16, 7),
                                                       gameModel::Position(13, 6), gameModel::Position(14, 6), gameModel::Position(15, 6), gameModel::Position(16, 6),
                                                       gameModel::Position(13, 5), gameModel::Position(14, 5), gameModel::Position(15, 5), gameModel::Position(16, 5)));
}

TEST(shot_test, throw_disperse_and_succed){
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {0, 1, 1, 0, 1}, {}});
    env->team1->chasers[0]->position = {6,6};
    env->quaffle->position = env->team1->chasers[0]->position;
    env->team1->chasers[1]->position = {7,7};
    env->team2->chasers[1]->position = {9,5};
    env->team1->chasers[2]->position = {7,5};
    env->team1->beaters[0]->position = {8,5};
    env->team1->beaters[1]->position = {8,7};
    env->team1->seeker->position = {9,6};
    env->team1->keeper->position = {9,7};
    env->team2->chasers[0]->position = {9,8};
    env->team2->chasers[2]->position = {7,6};
    gameController::Shot testShot = {env, env->team1->chasers[0], env->quaffle, {8,6}};
    testShot.execute();
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(6,8), gameModel::Position(7,8), gameModel::Position(8,8), gameModel::Position(9,8), gameModel::Position(10,8),
                                                       gameModel::Position(6,7), gameModel::Position(10,7), gameModel::Position(6,6), gameModel::Position(10,6),
                                                       gameModel::Position(6,5), gameModel::Position(10,5),
                                                       gameModel::Position(6,4), gameModel::Position(7,4), gameModel::Position(8,4), gameModel::Position(9,4), gameModel::Position(10,4)));
}

TEST(shot_test, shot_on_goal){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot testShot(env, env->team1->chasers[2], env->quaffle, {14, 8});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 2);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(res.first[1], gameController::ActionResult::ScoreLeft);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(14, 8));
}

TEST(shot_test, shot_on_goal1){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot testShot(env, env->team1->chasers[2], env->quaffle, {14, 4});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 2);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(res.first[1], gameController::ActionResult::ScoreLeft);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(14, 4));
}

TEST(shot_test, shot_on_goal2){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->team2->chasers[0]->position = {14,4};
    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Shot testShot (env, env->team1->chasers[0], env->quaffle, {14,4});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(),1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::Intercepted);
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(13,3), gameModel::Position(13,4), gameModel::Position(13,5),
                                                       gameModel::Position(14,3), gameModel::Position(14,5),
                                                       gameModel::Position(15,3), gameModel::Position(15,4), gameModel::Position(15,5)));
}

TEST(shot_test, invalid_shot_on_goal){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team2->chasers[0]->position;
    gameController::Shot testShot(env, env->team2->chasers[0], env->quaffle, {2, 8});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(2, 8));
}

TEST(shot_test, invalid_shot_on_goal1){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Shot testShot(env, env->team1->chasers[0], env->quaffle, {2, 8});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(2, 8));
}

TEST(shot_test, valid_throw_remove_shit){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->keeper->position;
    env->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(gameModel::Position{8, 6}));
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {8, 6});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(8, 6));
    EXPECT_TRUE(env->pileOfShit.empty());
}

TEST(shot_test, shot_through_goal){
    auto env = setup::createEnv({0, {}, {1, 0, 0, 0, 0}, {}});

    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot testShot(env, env->team1->chasers[2], env->quaffle, {15, 3});
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::ThrowSuccess);
    EXPECT_EQ(env->quaffle->position, gameModel::Position(15, 3));
}

TEST(shot_test, execute_all_standard_throw){
    auto env = setup::createEnv({0, {}, {0.5, 0, 0, 0, 0}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot shot(env, env->team1->chasers[2], env->quaffle, gameModel::Position{10, 5});
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 9);
    double sum = 0;
    std::deque<gameModel::Position> poses = {{10, 5}, {10, 6}, {11, 6}, {11, 5}, {11, 4}, {10, 4},
                                             {9, 4}, {9, 5}, {9, 6}};
    for(const auto &res : resList){
        sum += res.second;
        for(auto p = poses.begin(); p < poses.end();){
            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_DOUBLE_EQ(sum , 1);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
}

TEST(shot_test, execute_all_bouce_off){
    auto env = setup::createEnv({0, {}, {0.5, 0, 0, 0, 0}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    env->team1->seeker->position = {10, 5};
    gameController::Shot shot(env, env->team1->chasers[2], env->quaffle, gameModel::Position{10, 5});
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 8);

    double sum = 0;
    std::deque<gameModel::Position> poses = {{10, 6}, {11, 6}, {11, 5}, {11, 4}, {10, 4},
                                             {9, 4}, {9, 5}, {9, 6}};
    for(const auto &res : resList){
        sum += res.second;
        for(auto p = poses.begin(); p < poses.end();){
            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_DOUBLE_EQ(sum , 1);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
}

TEST(shot_test, execute_all_intercept){
    auto env = setup::createEnv({0, {}, {0.5, 0, 0, 0.4, 0}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    env->team2->keeper->position = {10, 6};
    gameController::Shot shot(env, env->team1->chasers[2], env->quaffle, gameModel::Position{10, 5});
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 9);

    double sum = 0;
    std::deque<gameModel::Position> poses = {{10, 5}, {11, 6}, {11, 5}, {11, 4}, {10, 4},
                                             {9, 4}, {9, 5}, {9, 6}, {10, 6}};
    for(const auto &res : resList){
        sum += res.second;
        for(auto p = poses.begin(); p < poses.end();){
            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_DOUBLE_EQ(sum , 1);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
}

TEST(shot_test, execute_all_intercept_with_seeker){
    auto env = setup::createEnv({0, {}, {0.5, 0, 0, 0.4, 0}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    env->team2->seeker->position = {10, 6};
    gameController::Shot shot(env, env->team1->chasers[2], env->quaffle, gameModel::Position{10, 5});
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 10);

    double sum = 0;
    std::deque<gameModel::Position> poses = {{10, 5}, {11, 6}, {11, 5}, {11, 4}, {10, 4},
                                             {9, 4}, {9, 5}, {9, 6}, {9, 7}, {11, 7}};
    for(const auto &res : resList){
        sum += res.second;
        for(auto p = poses.begin(); p < poses.end();){
            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_DOUBLE_EQ(sum , 1);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
}

TEST(shot_test, execute_all_long_shot){
    auto env = setup::createEnv({0, {}, {0.5, 0, 0, 0.4, 0}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    gameController::Shot shot(env, env->team1->chasers[2], env->quaffle, gameModel::Position{2, 7});
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 23);

    double sum = 0;
    std::deque<gameModel::Position> poses = {{1, 9}, {2, 9}, {3, 9}, {4, 9}, {0, 8}, {1, 8},
                                             {2, 8}, {3, 8}, {4, 8}, {0, 7}, {1, 7}, {2, 7},
                                             {3, 7}, {4, 7}, {1, 6}, {2, 6}, {3, 6}, {4, 6},
                                             {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}};
    for(const auto &res : resList){
        sum += res.second;
        for(auto p = poses.begin(); p < poses.end();){
            if(gameModel::Environment::isGoalCell(res.first->quaffle->position)){
                EXPECT_EQ(res.first->team2->score, gameController::GOAL_POINTS);
            }

            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_DOUBLE_EQ(sum , 1);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
    EXPECT_EQ(env->team1->score, 0);
    EXPECT_EQ(env->team2->score, 0);
}

TEST(shot_test, execute_all_long_shot_intercept){
    auto env = setup::createEnv({0, {}, {0.5, 0, 0, 0.4, 0}, {}});
    env->quaffle->position = env->team1->chasers[2]->position;
    env->team2->seeker->position = {7, 7};
    gameController::Shot shot(env, env->team1->chasers[2], env->quaffle, gameModel::Position{2, 7});
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 30);

    double sum = 0;
    std::deque<gameModel::Position> poses = {{1, 9}, {2, 9}, {3, 9}, {4, 9}, {0, 8}, {1, 8},
                                             {2, 8}, {3, 8}, {4, 8}, {0, 7}, {1, 7}, {2, 7},
                                             {3, 7}, {4, 7}, {1, 6}, {2, 6}, {3, 6}, {4, 6},
                                             {0, 5}, {1, 5}, {2, 5}, {3, 5}, {4, 5}, {6, 8},
                                             {7, 8}, {8, 8}, {6, 7}, {8, 7}, {6, 6}, {7, 6}};
    for(const auto &res : resList){
        sum += res.second;
        for(auto p = poses.begin(); p < poses.end();){
            if(gameModel::Environment::isGoalCell(res.first->quaffle->position)){
                EXPECT_EQ(res.first->team2->score, gameController::GOAL_POINTS);
            }

            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_NEAR(sum, 1, 0.0000001);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[2]->position);
    EXPECT_EQ(env->team1->score, 0);
    EXPECT_EQ(env->team2->score, 0);
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

TEST(shot_test, valid_bludger_shot_over_ball){
    auto env = setup::createEnv();
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    env->quaffle->position = {6, 2};
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], {7, 2});
    EXPECT_EQ(testShot.check(), gameController::ActionCheckResult::Success);
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
    auto env = setup::createEnv({0, {}, {0, 1, 0, 0, 0}, {}});
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->seeker->position);
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 1);
    EXPECT_EQ(res.first[0], gameController::ActionResult::Knockout);
    EXPECT_NE(env->bludgers[0]->position, env->team1->seeker->position);
    EXPECT_TRUE(env->team1->seeker->knockedOut);
}

TEST(shot_test, bludger_shot_on_Chaser_with_ball){
    using P = gameModel::Position;
    auto env = setup::createEnv({0, {}, {0, 1, 0, 0, 0}, {}});
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    env->quaffle->position = env->team2->chasers[0]->position;
    auto testShot = gameController::Shot(env, env->team2->beaters[1], env->bludgers[0], env->team2->chasers[0]->position);
    auto res = testShot.execute();
    EXPECT_EQ(res.first.size(), 2);
    EXPECT_EQ(res.first[0], gameController::ActionResult::Knockout);
    EXPECT_EQ(res.first[1], gameController::ActionResult::FoolAway);
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

TEST(shot_test, bludger_execute_all){
    auto env = setup::createEnv({0, {}, {0.5, 0.5, 0, 0.4, 0}, {}});
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    gameController::Shot shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->seeker->position);
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 2);
    EXPECT_TRUE(resList[0].first->team1->seeker->knockedOut);
    EXPECT_FALSE(resList[1].first->team1->seeker->knockedOut);
    EXPECT_DOUBLE_EQ(resList[0].second, env->config.getGameDynamicsProbs().knockOut);
    EXPECT_DOUBLE_EQ(resList[1].second, 1 - env->config.getGameDynamicsProbs().knockOut);
    EXPECT_NE(resList[0].first->bludgers[0]->position, env->team1->seeker->position);
    EXPECT_EQ(env->bludgers[0]->position, env->team2->beaters[1]->position);
    EXPECT_FALSE(env->team1->seeker->knockedOut);
}

TEST(shot_test, bludger_execute_all_fool_away){
    auto env = setup::createEnv({0, {}, {0.5, 0.5, 0, 0.4, 0}, {}});
    env->bludgers[0]->position = env->team2->beaters[1]->position;
    env->team1->chasers[0]->position = {5, 3};
    env->quaffle->position = env->team1->chasers[0]->position;
    gameController::Shot shot(env, env->team2->beaters[1], env->bludgers[0], env->team1->chasers[0]->position);
    auto resList = shot.executeAll();
    EXPECT_EQ(resList.size(), 7);

    double sum = 0;
    std::deque<gameModel::Position> poses = {{4, 4}, {6, 4}, {4, 3}, {6, 3}, {5, 2}, {6, 2}};
    for(const auto &res : resList){
        sum += res.second;
        if(res.first->team1->chasers[0]->knockedOut){
            for(auto p = poses.begin(); p < poses.end();){
                if(res.first->quaffle->position == *p){
                    p = poses.erase(p);
                } else {
                    p++;
                }
            }
        } else {
            EXPECT_EQ(res.first->quaffle->position, res.first->team1->chasers[0]->position);
        }
    }

    EXPECT_DOUBLE_EQ(sum, 1);
    EXPECT_TRUE(poses.empty());
    EXPECT_EQ(env->bludgers[0]->position, env->team2->beaters[1]->position);
    EXPECT_EQ(env->quaffle->position, env->team1->chasers[0]->position);
    EXPECT_FALSE(env->team1->chasers[0]->knockedOut);
}

//---------------------------getAllLandingCells Check Tests----------------------------------------------------------------------
TEST(shot_test, shot_test_get_all_landing_cells_Test1){
    auto env = setup::createEnv({0, {}, {0, 0, 0, 0, 0}, {}});

    env->team1->keeper->position = {9, 8};
    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {16, 8});
    auto res = testShot.execute();
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(15, 10), gameModel::Position(15, 9), gameModel::Position(15, 8), gameModel::Position(16, 7), gameModel::Position{15,7}));
}

TEST(shot_test, shot_test_get_all_landing_cells_Test2){
    auto env = setup::createEnv({0, {}, {0, 0, 0, 0, 0}, {}});

    env->team1->keeper->position = {0, 8};
    env->quaffle->position = env->team1->keeper->position;
    gameController::Shot testShot(env, env->team1->keeper, env->quaffle, {16, 8});
    auto res = testShot.execute();
    EXPECT_THAT(env->quaffle->position, testing::AnyOf(gameModel::Position(13,11), gameModel::Position(14,11),
                                                       gameModel::Position(13, 10), gameModel::Position(14, 10), gameModel::Position(15 ,10),
                                                       gameModel::Position(13, 9), gameModel::Position(14, 9), gameModel::Position(15, 9),
                                                       gameModel::Position(13, 8), gameModel::Position(14, 8), gameModel::Position(15, 8),
                                                       gameModel::Position(13, 7), gameModel::Position(14, 7), gameModel::Position(15, 7), gameModel::Position(16, 7),
                                                       gameModel::Position(13, 6), gameModel::Position(14, 6), gameModel::Position(15, 6), gameModel::Position(16, 6),
                                                       gameModel::Position(13, 5), gameModel::Position(14, 5), gameModel::Position(15, 5), gameModel::Position(16, 5)));
}

