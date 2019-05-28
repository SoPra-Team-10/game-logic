//
// Created by timluchterhand on 03.05.19.
//

#include "src/GameModel.h"

#ifndef SOPRAGAMELOGIC_SETUP_H
#define SOPRAGAMELOGIC_SETUP_H
namespace setup{
    auto createEnv() -> std::shared_ptr<gameModel::Environment>;
    auto createEnv(const gameModel::Config &config) -> std::shared_ptr<gameModel::Environment>;
}

#endif //SOPRAGAMELOGIC_SETUP_H
