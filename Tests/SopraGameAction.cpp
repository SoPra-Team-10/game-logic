//
// Created by jonas on 30.04.19.
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
                EXPECT_EQ(res.first->team2->score, GOAL_POINTS);
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
                EXPECT_EQ(res.first->team2->score, GOAL_POINTS);
            }

            if(res.first->quaffle->position == *p){
                p = poses.erase(p);
            } else {
                p++;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
    EXPECT_GE(sum, 0.9999999);
    EXPECT_LE(sum, 1.0000001);
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
    EXPECT_DOUBLE_EQ(resList[0].second, env->config.gameDynamicsProbs.knockOut);
    EXPECT_DOUBLE_EQ(resList[1].second, 1 - env->config.gameDynamicsProbs.knockOut);
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
    EXPECT_EQ(resList[0].first->team2->score, SNITCH_POINTS);
    EXPECT_EQ(resList[0].first->team2->seeker->position, gameModel::Position(12, 9));
    EXPECT_DOUBLE_EQ(resList[1].second, 1 - catchProb);
    EXPECT_EQ(resList[1].first->team2->score, 0);
    EXPECT_EQ(resList[1].first->team2->seeker->position, gameModel::Position(12, 9));
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
    EXPECT_EQ(resList[0].first->team1->score, GOAL_POINTS);
    EXPECT_FALSE(resList[0].first->team1->chasers[0]->isFined);
    EXPECT_DOUBLE_EQ(resList[0].second, 1 - env->config.foulDetectionProbs.chargeGoal);
    EXPECT_EQ(resList[1].first->quaffle->position, target);
    EXPECT_EQ(resList[1].first->team1->chasers[0]->position, target);
    EXPECT_EQ(resList[1].first->team1->score, GOAL_POINTS);
    EXPECT_DOUBLE_EQ(resList[1].second, env->config.foulDetectionProbs.chargeGoal);
    EXPECT_TRUE(resList[1].first->team1->chasers[0]->isFined);
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

    EXPECT_GE(sum, 0.999);
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

    EXPECT_GE(sum, 0.999);
}
//---------------------------WrestQuaffle Execute Move------------------------------------------------------------------
TEST(wrest_quaffel_test, wrest_execute0) {
    auto env = setup::createEnv();

    env->quaffle->position = env->team2->chasers[0]->position;
    env->team1->chasers[0]->position = gameModel::Position(6,0);

    gameController::WrestQuaffle action(env, env->team1->chasers[0], env->team2->chasers[0]->position);
    auto mvRes = action.execute();

    EXPECT_EQ(env->quaffle->position, env->team2->chasers[0]->position);
    EXPECT_TRUE(mvRes.first.empty());

}

TEST(wrest_quaffel_test, wrest_execute1) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->quaffle->position = env->team2->chasers[0]->position;
    env->team1->chasers[0]->position = gameModel::Position(6,0);

    gameController::WrestQuaffle action(env, env->team1->chasers[0], env->team2->chasers[0]->position);
    auto mvRes = action.execute();

    EXPECT_EQ(env->quaffle->position, gameModel::Position(6,0));
    EXPECT_FALSE(mvRes.first.empty());
    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::WrestQuaffel);

}

TEST(wrest_quaffel_test, wrest_execute2) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->team1->keeper->position = gameModel::Position(0, 4);
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[0]->position = gameModel::Position(0,5);

    gameController::WrestQuaffle action(env, env->team2->chasers[0], env->team1->keeper->position);
    EXPECT_ANY_THROW(action.execute());

}

TEST(wrest_quaffel_test, wrest_execute3) {
    auto env = setup::createEnv({0, {1, 1, 1, 1, 1, 1, 1, 1, 1, 1}, {1, 1, 1, 1, 1}, {}});

    env->team1->keeper->position = gameModel::Position(16, 4);
    env->quaffle->position = env->team1->keeper->position;
    env->team2->chasers[0]->position = gameModel::Position(16,5);

    gameController::WrestQuaffle action(env, env->team2->chasers[0], env->team1->keeper->position);
    auto mvRes = action.execute();

    EXPECT_EQ(env->quaffle->position, gameModel::Position(16,5));
    EXPECT_FALSE(mvRes.first.empty());
    EXPECT_EQ(mvRes.first[0], gameController::ActionResult::WrestQuaffel);

}