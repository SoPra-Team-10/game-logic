#include <utility>

#include <utility>

//
// Created by timluchterhand on 05.05.19.
//

#include "Interference.h"

namespace gameController{

    Interference::Interference(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
            gameModel::InterferenceType type) : env(std::move(env)), team(std::move(team)), type(type) {}

    void Teleport::execute() const {

    }

    bool Teleport::isPossible() const {
        return false;
    }

    Teleport::Teleport(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                       gameModel::Position target) : Interference(std::move(env), std::move(team),
                               gameModel::InterferenceType::Teleport), target(target) {}

    RangedAttack::RangedAttack(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                               gameModel::Position target) : Interference(std::move(env), std::move(team),
                                       gameModel::InterferenceType::RangedAttack), target(target){}

    void RangedAttack::execute() const {

    }

    bool RangedAttack::isPossible() const {
        return false;
    }

    Impulse::Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::Impulse){}

    void Impulse::execute() const {

    }

    bool Impulse::isPossible() const {
        return false;
    }

    SnitchPush::SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::SnitchPush){}

    void SnitchPush::execute() const {

    }

    bool SnitchPush::isPossible() const {
        return false;
    }
}
