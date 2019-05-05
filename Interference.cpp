#include <utility>

#include <utility>

//
// Created by timluchterhand on 05.05.19.
//

#include "Interference.h"

namespace gameController{

    Interference::Interference(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
            gameModel::InterferenceType type) : env(std::move(env)), team(std::move(team)), type(type) {}

    bool Interference::available() const {
        return team->fanblock.getUses(type) > 0;
    }

    void Teleport::execute() const {

    }

    bool Teleport::isPossible() const {
        return available() && env->getPlayer(target).has_value();
    }

    Teleport::Teleport(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                       gameModel::Position target) : Interference(std::move(env), std::move(team),
                               gameModel::InterferenceType::Teleport), target(target) {}

    auto Teleport::getType() const -> gameModel::InterferenceType {
        return type;
    }

    RangedAttack::RangedAttack(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                               gameModel::Position target) : Interference(std::move(env), std::move(team),
                                       gameModel::InterferenceType::RangedAttack), target(target){}

    void RangedAttack::execute() const {

    }

    bool RangedAttack::isPossible() const {
        auto player = env->getPlayer(target);
        if(available() && player.has_value()){
            return !team->hasMember(player.value());
        }

        return false;
    }

    auto RangedAttack::getType() const -> gameModel::InterferenceType {
        return type;
    }

    Impulse::Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::Impulse){}

    void Impulse::execute() const {

    }

    bool Impulse::isPossible() const {
        return available();
    }

    auto Impulse::getType() const -> gameModel::InterferenceType {
        return type;
    }

    SnitchPush::SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::SnitchPush){}

    void SnitchPush::execute() const {

    }

    bool SnitchPush::isPossible() const {
        return available();
    }

    auto SnitchPush::getType() const -> gameModel::InterferenceType {
        return type;
    }
}
