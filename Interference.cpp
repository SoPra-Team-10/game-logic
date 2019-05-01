#include <utility>

#include <utility>

#include <utility>

//
// Created by bjorn on 01.05.19.
//

#include "Interference.h"

Interference::Interference(std::shared_ptr<gameModel::Team> team) : team(std::move(team)) {}

Teleportation::Teleportation(std::shared_ptr<gameModel::Team> team) : Interference(std::move(team)) {}

bool Teleportation::isPossible(const gameModel::Environment &envi) {
    if(envi.team1.seeker.position.x == position.x && envi.team1.seeker.position.y == position.y) return false;
    else if(envi.team1.keeper.position.x == position.x && envi.team1.keeper.position.y == position.y) return false;
    else if(envi.team1.beaters[0].position.x == position.x && envi.team1.beaters[0].position.y == position.y) return false;
    else if(envi.team1.beaters[1].position.x == position.x && envi.team1.beaters[1].position.y == position.y) return false;
    else if(envi.team1.chasers[0].position.x == position.x && envi.team1.chasers[0].position.y == position.y) return false;
    else if(envi.team1.chasers[1].position.x == position.x && envi.team1.chasers[1].position.y == position.y) return false;
    else if(envi.team1.chasers[2].position.x == position.x && envi.team1.chasers[2].position.y == position.y) return false;
    else return true;
}
void Teleportation::execute(gameModel::Environment &envi) {}
std::vector<gameModel::Environment> Teleportation::executeAll(const gameModel::Environment &envi) {}

RangedAttack::RangedAttack(const std::shared_ptr<gameModel::Team>& team) : Interference(team) {}

bool RangedAttack::isPossible(const gameModel::Environment &envi) {}
void RangedAttack::execute(gameModel::Environment &envi) {}
std::vector<gameModel::Environment> RangedAttack::executeAll(const gameModel::Environment &envi) {}

Impulse::Impulse(std::shared_ptr<gameModel::Team> team) : Interference(std::move(team)) {}

bool Impulse::isPossible(const gameModel::Environment &envi) {}
void Impulse::execute(gameModel::Environment &envi) {}
std::vector<gameModel::Environment> Impulse::executeAll(const gameModel::Environment &envi) {}

