//
// Created by bjorn on 01.05.19.
//

#ifndef SOPRAGAMELOGIC_INTERFERENCE_H
#define SOPRAGAMELOGIC_INTERFERENCE_H


#include <memory>
#include "sopraGameModel.h"
#include "vector"

class Interference {
public:
    std::shared_ptr<gameModel::Team> team;

    Interference() = default;
    explicit Interference(std::shared_ptr<gameModel::Team> team);

    virtual bool isPossible(const gameModel::Environment &envi) = 0;
    virtual void execute(gameModel::Environment &envi) = 0;
    virtual std::vector<gameModel::Environment> executeAll(const gameModel::Environment &envi) = 0;

};

class Teleportation : public Interference{
public:
    gameModel::Position position{};

    Teleportation() = default;
    explicit Teleportation(std::shared_ptr<gameModel::Team> team);

    bool isPossible(const gameModel::Environment &envi) override;
    void execute(gameModel::Environment &envi) override;
    std::vector<gameModel::Environment> executeAll(const gameModel::Environment &envi) override;
};

class RangedAttack : public Interference{
public:
    RangedAttack() = default;
    explicit RangedAttack(const std::shared_ptr<gameModel::Team>& team);

    bool isPossible(const gameModel::Environment &envi) override;

    std::vector<gameModel::Environment> executeAll(const gameModel::Environment &envi) override;

    void execute(gameModel::Environment &envi) override;
};

class Impulse : Interference{
public:
    Impulse() = default;
    Impulse(std::shared_ptr<gameModel::Team> team);

    bool isPossible(const gameModel::Environment &envi) override;
    void execute(gameModel::Environment &envi) override;
    std::vector<gameModel::Environment> executeAll(const gameModel::Environment &envi) override;
};
#endif //SOPRAGAMELOGIC_INTERFERENCE_H
