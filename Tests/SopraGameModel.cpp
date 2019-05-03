//
// Created by timluchterhand on 29.04.19.
//

#include <gtest/gtest.h>
#include "GameModel.h"

TEST(env_tests, getCell){
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
}

TEST(env_tests, cellIsFree){
    using ID = communication::messages::types::EntityId;
    gameModel::Config conf(0, {}, {}, {}, {});
    gameModel::Chaser c1({2, 10}, "", {}, {}, ID::LEFT_CHASER1);
    gameModel::Chaser c2({8, 5}, "", {}, {}, ID::LEFT_CHASER2);
    gameModel::Chaser c3({10, 7}, "", {}, {}, ID::RIGHT_CHASER3);
    gameModel::Chaser c4({6, 1}, "", {}, {}, ID::RIGHT_CHASER1);
    gameModel::Chaser c5({9, 9}, "", {}, {}, ID::RIGHT_CHASER2);
    gameModel::Chaser c6({7, 3}, "", {}, {}, ID::RIGHT_CHASER3);

    gameModel::Beater b1({1, 3}, "", {}, {}, ID::LEFT_BEATER1);
    gameModel::Beater b2({3, 0}, "", {}, {}, ID::LEFT_BEATER2);
    gameModel::Beater b3({0, 6}, "", {}, {}, ID::RIGHT_BEATER1);
    gameModel::Beater b4({4, 2}, "", {}, {}, ID::RIGHT_BEATER2);

    gameModel::Seeker s1({5, 4}, "", {}, {}, ID::LEFT_SEEKER);
    gameModel::Seeker s2({11, 8}, "", {}, {}, ID::RIGHT_SEEKER);

    gameModel::Keeper k1({12, 11}, "", {}, {}, ID::LEFT_KEEPER);
    gameModel::Keeper k2({13, 12}, "", {}, {}, ID::RIGHT_KEEPER);

    gameModel::Fanblock f(1, 2, 3, 1);

    gameModel::Team t1(s1, k1, {b1, b2}, {c1, c2, c3}, "", "", "", f);
    gameModel::Team t2(s2, k2, {b3, b4}, {c4, c5, c6}, "", "", "", f);

    gameModel::Environment env(conf, t1, t2);

    EXPECT_FALSE(env.cellIsFree(c1.position));
    EXPECT_FALSE(env.cellIsFree(c2.position));
    EXPECT_FALSE(env.cellIsFree(c3.position));
    EXPECT_FALSE(env.cellIsFree(c4.position));
    EXPECT_FALSE(env.cellIsFree(c5.position));
    EXPECT_FALSE(env.cellIsFree(c6.position));
    EXPECT_FALSE(env.cellIsFree(b1.position));
    EXPECT_FALSE(env.cellIsFree(b2.position));
    EXPECT_FALSE(env.cellIsFree(b3.position));
    EXPECT_FALSE(env.cellIsFree(b4.position));
    EXPECT_FALSE(env.cellIsFree(k1.position));
    EXPECT_FALSE(env.cellIsFree(k2.position));
    EXPECT_FALSE(env.cellIsFree(s1.position));
    EXPECT_FALSE(env.cellIsFree(s2.position));

    EXPECT_TRUE(env.cellIsFree({8, 6}));
    EXPECT_TRUE(env.cellIsFree({2, 6}));
    EXPECT_TRUE(env.cellIsFree({2, 9}));
    EXPECT_TRUE(env.cellIsFree({11, 7}));
    EXPECT_TRUE(env.cellIsFree({5, 10}));
    EXPECT_TRUE(env.cellIsFree({4, 1}));
    EXPECT_TRUE(env.cellIsFree({10, 2}));
}
