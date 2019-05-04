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

TEST(move_test, move_foul0) {
    auto env = setup::createEnv();

    if (env.getPlayer(gameModel::Position(8, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(8, 6), std::make_shared<gameModel::Environment>(env));
    }
    env.team1.keeper.position = gameModel::Position(8, 6);

    if (env.getPlayer(gameModel::Position(7, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(7, 6), std::make_shared<gameModel::Environment>(env));
    }
    env.team2.keeper.position = gameModel::Position(7, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Keeper>(env.team2.keeper),
                            gameModel::Position(8, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::Ramming);
}

TEST(move_test, move_foul1) {
    auto env = setup::createEnv();

    if (env.getPlayer(gameModel::Position(8, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(8, 6), std::make_shared<gameModel::Environment>(env));
    }
    env.team2.seeker.position = gameModel::Position(8, 6);

    if (env.getPlayer(gameModel::Position(7, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(7, 6), std::make_shared<gameModel::Environment>(env));
    }
    env.team2.keeper.position = gameModel::Position(7, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Keeper>(env.team2.keeper),
                            gameModel::Position(8, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::None);
}

TEST(move_test, move_foul2) {
    auto env = setup::createEnv();

    if (env.getPlayer(gameModel::Position(8, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(8, 6), std::make_shared<gameModel::Environment>(env));
    }
    env.snitch.position = gameModel::Position(8, 6);

    if (env.getPlayer(gameModel::Position(7, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(7, 6), std::make_shared<gameModel::Environment>(env));
    }
    env.team2.keeper.position = gameModel::Position(7, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Keeper>(env.team2.keeper),
                            gameModel::Position(8, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::BlockSnitch);
}

TEST(move_test, move_foul3) {
    auto env = setup::createEnv();

    env.team1.chasers[0].position = gameModel::Position(16, 7);
    env.team1.chasers[1].position = gameModel::Position(11, 7);

    if (env.getPlayer(gameModel::Position(12, 7)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(12, 7), std::make_shared<gameModel::Environment>(env));
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Chaser>(env.team1.chasers[1]),
                            gameModel::Position(12, 7));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::MultibleOffence);
}

TEST(move_test, move_foul4) {
    auto env = setup::createEnv();

    env.team1.chasers[0].position = gameModel::Position(1, 6);

    if (env.getPlayer(gameModel::Position(2, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(2, 6), std::make_shared<gameModel::Environment>(env));
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Chaser>(env.team1.chasers[0]),
                            gameModel::Position(2, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::BlockGoal);
}

TEST(move_test, move_foul5) {
    auto env = setup::createEnv();

    env.team2.chasers[0].position = gameModel::Position(1, 6);
    env.quaffle.position = gameModel::Position(1, 6);

    if (env.getPlayer(gameModel::Position(2, 6)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(2, 6), std::make_shared<gameModel::Environment>(env));
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Chaser>(env.team2.chasers[0]),
                            gameModel::Position(2, 6));

    EXPECT_EQ(mv.checkForFoul(), gameModel::Foul::ChargeGoal);
}

TEST(move_test, move_check0) {
    auto env = setup::createEnv();

    env.team1.keeper.position = gameModel::Position(7, 1);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Keeper>(env.team1.keeper),
                            gameModel::Position(7, 3));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Impossible);

}

TEST(move_test, move_check1) {
    auto env = setup::createEnv();

    env.team1.keeper.position = gameModel::Position(7, 1);

    if (env.getPlayer(gameModel::Position(7, 2)).has_value()) {
        gameController::movePlayerOnEmptyCell(gameModel::Position(7, 2), std::make_shared<gameModel::Environment>(env));
    }

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Keeper>(env.team1.keeper),
                            gameModel::Position(7, 2));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Success);
}

TEST(move_test, move_check2) {
    auto env = setup::createEnv();

    env.team1.chasers[0].position = gameModel::Position(16, 6);
    env.team1.chasers[1].position = gameModel::Position(11, 6);

    gameController::Move mv(std::make_shared<gameModel::Environment>(env),
                            std::make_shared<gameModel::Chaser>(env.team1.chasers[1]),
                            gameModel::Position(12, 6));

    EXPECT_EQ(mv.check(), gameController::ActionResult::Foul);
}