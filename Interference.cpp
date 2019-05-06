//
// Created by timluchterhand on 05.05.19.
//

#include "Interference.h"

namespace gameController{

    Interference::Interference(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
            gameModel::InterferenceType type) : env(std::move(env)), team(std::move(team)), type(type) {}

    auto Interference::getType() const -> gameModel::InterferenceType {
        return type;
    }

    bool Interference::isPossible() const {
        return team->fanblock.getUses(type) > 0;
    }

    Teleport::Teleport(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                       std::shared_ptr<gameModel::Player> target) : Interference(std::move(env), std::move(team),
                                                                  gameModel::InterferenceType::Teleport), target(std::move(target)) {}
    void Teleport::execute() const {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        auto possibleCells = env->getAllFreeCells();
        target->position = possibleCells[rng(0, static_cast<int>(possibleCells.size() - 1))];
    }

    RangedAttack::RangedAttack(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                               std::shared_ptr<gameModel::Player> target) : Interference(std::move(env), std::move(team),
                                       gameModel::InterferenceType::RangedAttack), target(std::move(target)){}

    void RangedAttack::execute() const {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        if(env->quaffle->position == target->position){
            moveToAdjacent(env->quaffle, env);
        }

        moveToAdjacent(target, env);
    }

    bool RangedAttack::isPossible() const {
        return Interference::isPossible() && !team->hasMember(target);
    }

    Impulse::Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::Impulse){}

    void Impulse::execute() const {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        for(const auto &player : env->getAllPlayers()){
            if(player->position == env->quaffle->position &&
                (INSTANCE_OF(player, gameModel::Chaser) ||
                INSTANCE_OF(player, gameModel::Keeper))) {
                moveToAdjacent(env->quaffle, env);
                break;
            }
        }
    }

    SnitchPush::SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::SnitchPush){}

    void SnitchPush::execute() const {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        if(env->snitch->exists){
            moveToAdjacent(env->snitch, env);
        }
    }
}
