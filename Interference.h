//
// Created by timluchterhand on 05.05.19.
//

#ifndef SOPRAGAMELOGIC_INTERFERENCE_H
#define SOPRAGAMELOGIC_INTERFERENCE_H

#include <memory>
#include <cmath>
#include <vector>
#include "GameController.h"
#include "GameModel.h"
namespace gameController{
    class Interference {
    public:
        Interference(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                gameModel::InterferenceType type);

        virtual void execute() const = 0;
        virtual bool isPossible() const = 0;
    protected:
        std::shared_ptr<gameModel::Environment> env;
        std::shared_ptr<gameModel::Team> team;
        gameModel::InterferenceType type;
    };

    class Teleport : public Interference {
    public:
        Teleport(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                 gameModel::Position target);
        void execute() const override;
        bool isPossible() const override;

    private:
        gameModel::Position target;
    };

    class RangedAttack : public Interference {
    public:
        RangedAttack(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                gameModel::Position target);

        void execute() const override;
        bool isPossible() const override;
    private:
        gameModel::Position target;
    };

    class Impulse : public Interference {
    public:
    Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team);

        void execute() const override;
        bool isPossible() const override;
    };

    class SnitchPush : public Interference {
    public:
        SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team);

        void execute() const override;
        bool isPossible() const override;
    };
}


#endif //SOPRAGAMELOGIC_INTERFERENCE_H
