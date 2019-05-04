//
// Created by timluchterhand on 03.05.19.
//

#include "GameModel.h"

#ifndef SOPRAGAMELOGIC_SETUP_H
#define SOPRAGAMELOGIC_SETUP_H
namespace setup{
    auto createEnv() -> gameModel::Environment;
    auto createEnv(const gameModel::Config &config) -> gameModel::Environment;
}

#endif //SOPRAGAMELOGIC_SETUP_H
