//
// Created by timluchterhand on 03.05.19.
//

#include "setup.h"

auto setup::createEnv() -> gameModel::Environment {
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
    return env;
}