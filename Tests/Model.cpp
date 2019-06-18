//
// Created by timluchterhand on 29.04.19.
//

#include <gtest/gtest.h>
#include <gmock/gmock-matchers.h>
#include <Interference.h>
#include "GameModel.h"
#include "GameController.h"
#include "setup.h"

//-----------------------------------------Environment Test-------------------------------------------------------------

TEST(env_test, getCell){
    EXPECT_EQ(gameModel::Environment::getCell(6, 3), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(7, 8), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(3, 0), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(3, 1), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(12, 9), gameModel::Cell::Standard);
    EXPECT_EQ(gameModel::Environment::getCell(5, 5), gameModel::Cell::Standard);

    EXPECT_EQ(gameModel::Environment::getCell(7, 7), gameModel::Cell::Centre);
    EXPECT_EQ(gameModel::Environment::getCell(8, 6), gameModel::Cell::Centre);

    EXPECT_EQ(gameModel::Environment::getCell(0, 7), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(3, 8), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(1, 6), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(3, 3), gameModel::Cell::RestrictedLeft);
    EXPECT_EQ(gameModel::Environment::getCell(2, 11), gameModel::Cell::RestrictedLeft);

    EXPECT_EQ(gameModel::Environment::getCell(14, 1), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(15, 2), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(15, 4), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(13, 9), gameModel::Cell::RestrictedRight);
    EXPECT_EQ(gameModel::Environment::getCell(14, 11), gameModel::Cell::RestrictedRight);

    EXPECT_EQ(gameModel::Environment::getCell(8, -1), gameModel::Cell::OutOfBounds);
    EXPECT_EQ(gameModel::Environment::getCell(-1, 4), gameModel::Cell::OutOfBounds);
}

TEST(env_test, cellIsFree0){
    auto env = setup::createEnv();

    EXPECT_FALSE(env->cellIsFree(env->team1->chasers[0]->position));
    EXPECT_FALSE(env->cellIsFree(env->team1->chasers[1]->position));
    EXPECT_FALSE(env->cellIsFree(env->team1->chasers[2]->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->chasers[0]->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->chasers[1]->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->chasers[2]->position));
    EXPECT_FALSE(env->cellIsFree(env->team1->beaters[0]->position));
    EXPECT_FALSE(env->cellIsFree(env->team1->beaters[1]->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->beaters[0]->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->beaters[1]->position));
    EXPECT_FALSE(env->cellIsFree(env->team1->keeper->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->keeper->position));
    EXPECT_FALSE(env->cellIsFree(env->team1->seeker->position));
    EXPECT_FALSE(env->cellIsFree(env->team2->seeker->position));

    EXPECT_TRUE(env->cellIsFree({2, 6}));
    EXPECT_TRUE(env->cellIsFree({2, 9}));
    EXPECT_TRUE(env->cellIsFree({11, 7}));
    EXPECT_TRUE(env->cellIsFree({5, 10}));
    EXPECT_TRUE(env->cellIsFree({4, 1}));
    EXPECT_TRUE(env->cellIsFree({10, 2}));
}


TEST(env_test, cellIsFree1) {
    auto env = setup::createEnv();

    int x = gameController::rng(5, 11);
    int y = gameController::rng(0, 12);

    auto player = env->getPlayer({x, y});
    if (player.has_value() && !player.value()->isFined) {
        gameController::moveToAdjacent(player.value(), env);
    }

    if(env->snitch->exists && env->snitch->position == gameModel::Position{x, y}){
        gameController::moveToAdjacent(env->snitch, env);
    }

    if(env->bludgers[0]->position == gameModel::Position{x, y}){
        gameController::moveToAdjacent(env->bludgers[0], env);
    }

    if(env->bludgers[1]->position == gameModel::Position{x, y}){
        gameController::moveToAdjacent(env->bludgers[1], env);
    }

    if(env->quaffle->position == gameModel::Position{x, y}){
        gameController::moveToAdjacent(env->quaffle, env);
    }

    EXPECT_TRUE(env->cellIsFree(gameModel::Position(x,y)));

    env->team1->keeper->position = gameModel::Position(x,y);

    EXPECT_FALSE(env->cellIsFree(gameModel::Position(x,y)));

}

TEST(env_test, cellIsFree2){
    auto env = setup::createEnv();
    env->team1->seeker->isFined = true;
    env->team1->keeper->position = env->team1->seeker->position;
    EXPECT_FALSE(env->cellIsFree(env->team1->seeker->position));
}


TEST(env_test, cellIsFreeBall){
    auto env = setup::createEnv();
    env->snitch->position = {11, 2};
    env->snitch->exists = true;
    env->bludgers[0]-> position = {6, 9};
    env->bludgers[1]-> position = {2, 7};
    EXPECT_FALSE(env->cellIsFree({11, 2}));
    EXPECT_FALSE(env->cellIsFree({6, 9}));
    EXPECT_FALSE(env->cellIsFree({2, 7}));
    env->snitch->exists = false;
    EXPECT_TRUE(env->cellIsFree({11, 2}));
}

TEST(env_test, arePlayerInSameTeam) {
    auto env = setup::createEnv();

    auto team1Players = env->team1->getAllPlayers();
    auto team2Players = env->team2->getAllPlayers();

    EXPECT_TRUE(env->arePlayerInSameTeam(team1Players[gameController::rng(0, 6)], team1Players[gameController::rng(0, 6)]));
    EXPECT_TRUE(env->arePlayerInSameTeam(team2Players[gameController::rng(0, 6)], team2Players[gameController::rng(0, 6)]));
    EXPECT_FALSE(env->arePlayerInSameTeam(team1Players[gameController::rng(0, 6)], team2Players[gameController::rng(0, 6)]));
}

TEST(env_test, isPlayerInOwnRestrictedZone) {
    auto env = setup::createEnv();


    env->team1->keeper->position = gameModel::Position(3, 6);
    env->team2->keeper->position = gameModel::Position(3, 6);
    env->team1->seeker->position = gameModel::Position(2, 4);
    env->team2->seeker->position = gameModel::Position(2, 4);

    EXPECT_TRUE(env->isPlayerInOwnRestrictedZone(env->team1->keeper));
    EXPECT_FALSE(env->isPlayerInOwnRestrictedZone(env->team2->keeper));
    EXPECT_FALSE(env->isPlayerInOpponentRestrictedZone(env->team1->seeker));
    EXPECT_TRUE(env->isPlayerInOpponentRestrictedZone(env->team2->seeker));
}

TEST(env_test, isPlayerInOpponentRestrictedZone) {
    auto env = setup::createEnv();

    env->team1->keeper->position = gameModel::Position(3, 6);
    env->team2->keeper->position = gameModel::Position(3, 6);
    env->team1->seeker->position = gameModel::Position(2, 4);
    env->team2->seeker->position = gameModel::Position(2, 4);

    EXPECT_FALSE(env->isPlayerInOpponentRestrictedZone(env->team1->keeper));
    EXPECT_TRUE(env->isPlayerInOpponentRestrictedZone(env->team2->keeper));
    EXPECT_FALSE(env->isPlayerInOpponentRestrictedZone(env->team1->seeker));
    EXPECT_TRUE(env->isPlayerInOpponentRestrictedZone(env->team2->seeker));
}

TEST(env_test, getTeamMates) {
    auto env = setup::createEnv();

    auto team = env->getTeamMates(env->team1->keeper);

    for (auto player : team) {
        EXPECT_TRUE(env->arePlayerInSameTeam(player, env->team1->keeper));
    }
}

TEST(env_test, getOpponents) {
    auto env = setup::createEnv();

    auto team = env->getOpponents(env->team1->keeper);

    for (auto player : team) {
        EXPECT_FALSE(env->arePlayerInSameTeam(player, env->team1->keeper));
    }
}

TEST(env_test, getSurroundingPositions){
    using env = gameModel::Environment;
    using P = gameModel::Position;
    auto positions = env::getSurroundingPositions({10, 2});
    EXPECT_EQ(positions.size(), 8);
    for(const auto &pos : positions){
        EXPECT_THAT(pos, testing::AnyOf(P{10, 1}, P{9, 1}, P{9, 2}, P{9, 3}, P{10, 3}, P{11, 3}, P{11, 2}, P{11, 1}));
    }
}

TEST(env_test, getSurroundingPositions_corner){
    using env = gameModel::Environment;
    using P = gameModel::Position;
    auto positions = env::getSurroundingPositions({2, 1});
    EXPECT_EQ(positions.size(), 5);
    for(const auto &pos : positions){
        EXPECT_THAT(pos, testing::AnyOf(P{1, 2}, P{2, 2}, P{3, 2}, P{3, 1}, P{3, 0}));
    }
}

TEST(env_test, getAllPlayerFreeCellsAround) {
    auto env = setup::createEnv();

    env->team1->chasers[0]->position = gameModel::Position(12, 12);
    env->team1->chasers[1]->position = gameModel::Position(13, 11);
    env->team1->chasers[2]->position = gameModel::Position(14, 11);

    auto freeCells = env->getAllFreeCellsAround(env->team2->keeper->position);

    EXPECT_EQ(freeCells.size(), 7);

    EXPECT_EQ(freeCells[0], gameModel::Position(11, 10));
    EXPECT_EQ(freeCells[1], gameModel::Position(12, 10));
    EXPECT_EQ(freeCells[2], gameModel::Position(13, 10));
    EXPECT_EQ(freeCells[3], gameModel::Position(14, 10));
    EXPECT_EQ(freeCells[4], gameModel::Position(15, 10));
    EXPECT_EQ(freeCells[5], gameModel::Position(11, 11));
    EXPECT_EQ(freeCells[6], gameModel::Position(11, 12));
}

TEST(env_test, isShitOnCell){
    auto env = setup::createEnv();
    gameController::BlockCell testShit(env, env->team1, gameModel::Position(5,6));
    testShit.execute();
    EXPECT_TRUE(env->isShitOnCell(gameModel::Position(5,6)));
}

TEST(env_test, removeShitOnCell0){
    auto env = setup::createEnv();
    gameController::BlockCell testShit(env, env->team1, gameModel::Position(8,7));
    testShit.execute();
    env->removeShitOnCell(gameModel::Position(8,7));
    EXPECT_TRUE(env->pileOfShit.empty());
}

TEST(env_test, removeShitOnCell1){
    auto env = setup::createEnv();
    gameController::BlockCell testShit(env, env->team1, gameModel::Position(8,7));
    testShit.execute();
    env->removeShitOnCell(gameModel::Position(8,6));
    EXPECT_FALSE(env->pileOfShit.empty());
}

TEST(env_test, removeDeprecatedShit){
    auto env = setup::createEnv();
    env->snitch->position  = gameModel::Position(5,5);
    env->quaffle->position  = gameModel::Position(5,6);
    env->bludgers[0]->position  = gameModel::Position(5,7);
    env->bludgers[1]->position  = gameModel::Position(5,8);
    gameController::BlockCell testShit(env, env->team1, gameModel::Position(8,7));
    testShit.execute();
    env->removeDeprecatedShit();
    EXPECT_FALSE(env->pileOfShit.empty());
    env->removeDeprecatedShit();
    EXPECT_TRUE(env->pileOfShit.empty());
}


TEST(env_test, clone){
    auto originalEnv = setup::createEnv();
    auto newEnv = originalEnv->clone();
    newEnv->team1->score = 30;
    newEnv->team2->seeker->isFined = true;
    newEnv->team2->keeper->position = {10, 0};
    newEnv->team1->chasers[0]->knockedOut = true;
    newEnv->team1->fanblock.banFan(gameModel::InterferenceType::BlockCell);
    newEnv->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(gameModel::Position(10, 2)));

    EXPECT_EQ(originalEnv->team1->score, 0);
    EXPECT_FALSE(originalEnv->team2->seeker->isFined);
    EXPECT_EQ(originalEnv->team2->keeper->position, gameModel::Position(13, 12));
    EXPECT_FALSE(originalEnv->team1->chasers[0]->knockedOut);
    EXPECT_EQ(originalEnv->team1->fanblock.getBannedCount(gameModel::InterferenceType::BlockCell), 0);
    EXPECT_TRUE(originalEnv->pileOfShit.empty());
}

TEST(env_test, legalCells){
    auto env = setup::createEnv();
    env->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(gameModel::Position{1, 6}));
    auto res = env->getAllLegalCellsAround({1, 7}, true);
    EXPECT_EQ(res.size(), 4);
    std::deque<gameModel::Position> poses = {gameModel::Position(0, 7), gameModel::Position(0, 8),
                                             gameModel::Position(1, 8), gameModel::Position(2, 7)};
    for(const auto &cell : res){
        for(auto it = poses.begin(); it  < poses.end(); it++){
            if(cell == *it){
                poses.erase(it);
                break;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
}

TEST(env_test, getAllEmptyCellsAround){
    auto env = setup::createEnv();
    env->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(gameModel::Position{9, 8}));
    auto res = env->getAllEmptyCellsAround(env->team1->chasers[2]->position);
    EXPECT_EQ(res.size(), 6);
    std::deque<gameModel::Position> poses = {gameModel::Position(9, 6), gameModel::Position(9, 7), gameModel::Position(10, 6),
                                             gameModel::Position(10, 8), gameModel::Position(11, 6), gameModel::Position(11, 7)};
    for(const auto &cell : res){
        for(auto it = poses.begin(); it < poses.end(); it++){
            if(cell == *it){
                poses.erase(it);
                break;
            }
        }
    }

    EXPECT_TRUE(poses.empty());
}

//-----------------------------------------Fanblock Test----------------------------------------------------------------

TEST(fanblock_test, banFan_and_getUses_and_getBannedCount) {
    auto env = setup::createEnv();
    // team1 has 3x impulse interferences

    env->team1->fanblock.banFan(gameModel::InterferenceType::Impulse);

    EXPECT_EQ(env->team1->fanblock.getBannedCount(gameModel::InterferenceType::Impulse), 1);
    EXPECT_EQ(env->team1->fanblock.getUses(gameModel::InterferenceType::Impulse), 1);
}