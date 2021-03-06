/**
 * @file Interference.cpp
 * @author Björn, Tim
 * @date 05.05.19
 * @brief Implementation of the Interferences.
 */

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
    auto Teleport::execute() const -> gameController::ActionCheckResult {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        auto possibleCells = env->getAllFreeCells();
        target->position = possibleCells[rng(0, static_cast<int>(possibleCells.size() - 1))];

        if (gameController::actionTriggered(env->config.getFoulDetectionProb(type))) {
            team->fanblock.banFan(this->getType());
            return gameController::ActionCheckResult::Foul;
        }
        else {
            return gameController::ActionCheckResult::Success;
        }
    }

    bool Teleport::isPossible() const {
        const bool isInField = gameModel::Environment::getCell(target->position) != gameModel::Cell::OutOfBounds;
        return Interference::isPossible() && isInField && !target->isFined;
    }

    RangedAttack::RangedAttack(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                               std::shared_ptr<gameModel::Player> target) : Interference(std::move(env), std::move(team),
                                       gameModel::InterferenceType::RangedAttack), target(std::move(target)){}

    auto RangedAttack::execute() const -> gameController::ActionCheckResult {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        if(env->quaffle->position == target->position){
            moveToAdjacent(env->quaffle, env);
        }

        moveToAdjacent(target, env);

        if (gameController::actionTriggered(env->config.getFoulDetectionProb(type))) {
            team->fanblock.banFan(this->getType());
            return gameController::ActionCheckResult::Foul;
        }
        else {
            return gameController::ActionCheckResult::Success;
        }
    }

    bool RangedAttack::isPossible() const {
        const bool isInField = gameModel::Environment::getCell(target->position) != gameModel::Cell::OutOfBounds;
        return Interference::isPossible() && !team->hasMember(target) && isInField && !target->isFined;
    }

    Impulse::Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::Impulse){}

    auto Impulse::execute() const -> gameController::ActionCheckResult {
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

        if (gameController::actionTriggered(env->config.getFoulDetectionProb(type))) {
            team->fanblock.banFan(this->getType());
            return gameController::ActionCheckResult::Foul;
        }
        else {
            return gameController::ActionCheckResult::Success;
        }
    }

    SnitchPush::SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team) :
        Interference(std::move(env), std::move(team), gameModel::InterferenceType::SnitchPush){}

    auto SnitchPush::execute() const -> gameController::ActionCheckResult {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }

        if(env->snitch->exists){
            moveToAdjacent(env->snitch, env);
        }

        if (gameController::actionTriggered(env->config.getFoulDetectionProb(type))) {
            team->fanblock.banFan(this->getType());
            return gameController::ActionCheckResult::Foul;
        }
        else {
            return gameController::ActionCheckResult::Success;
        }
    }

    BlockCell::BlockCell(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team, gameModel::Position target) :
            Interference(std::move(env), std::move(team), gameModel::InterferenceType::BlockCell), target(target) {}

    auto BlockCell::execute() const -> gameController::ActionCheckResult {
        if(!isPossible()){
            throw std::runtime_error("Interference not possible");
        }
        env->pileOfShit.emplace_back(std::make_shared<gameModel::CubeOfShit>(target));
        if (gameController::actionTriggered(env->config.getFoulDetectionProb(type))) {
            team->fanblock.banFan(this->getType());
            return gameController::ActionCheckResult::Foul;
        }
        else {
            return gameController::ActionCheckResult::Success;
        }
    }

    bool BlockCell::isPossible() const {
        const bool isInField = gameModel::Environment::getCell(target) != gameModel::Cell::OutOfBounds;
        return Interference::isPossible() && Interference::env->cellIsFree(target) && isInField;
    }
}
