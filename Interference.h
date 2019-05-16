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

        /**
         * Executes the interference
         */
        virtual auto execute() const -> gameController::ActionCheckResult = 0;

        /**
         * Checks if the interference is possible
         * @return
         */
        virtual bool isPossible() const;

        /**
         * Gets the type of interference
         * @return
         */
        auto getType() const -> gameModel::InterferenceType;
    protected:

        std::shared_ptr<gameModel::Environment> env;
        std::shared_ptr<gameModel::Team> team;
        gameModel::InterferenceType type;
    };

    class Teleport : public Interference {
    public:
        Teleport(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                 std::shared_ptr<gameModel::Player> target);

        /**
         * Teleports target player to random free location on the field
         */
        auto execute() const -> gameController::ActionCheckResult override;

        bool isPossible() const override;

    private:
        std::shared_ptr<gameModel::Player> target;
    };

    class RangedAttack : public Interference {
    public:
        RangedAttack(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                std::shared_ptr<gameModel::Player> target);

        /**
         * Pushes target player to a random free adjacent position
         * if target player previously held quaffle, quaffle will be moved to random free adjacent position
         */
        auto execute() const -> gameController::ActionCheckResult override;

        /**
         *
         * @return true if available and opponent target, false otherwise
         */
        bool isPossible() const override;
    private:
        std::shared_ptr<gameModel::Player> target;
    };

    class Impulse : public Interference {
    public:
        Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team);

        /**
         * If a Keeper or Chaser holds the quaffle, quaffle is moved to a random free adjacent position
         */
        auto execute() const -> gameController::ActionCheckResult override;

    };

    class SnitchPush : public Interference {
    public:
        SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team);

        /**
         * Snitch is moved to random free adjacent position
         */
        auto execute() const -> gameController::ActionCheckResult override;
    };

    class BlockCell : public Interference{
    public:
        BlockCell(std::shared_ptr<gameModel::Environment> env, const std::shared_ptr<gameModel::Team>& team, gameModel::Position position);

        bool isPossible() const override ;
    private:
        gameModel::Position position;
        auto execute() const -> gameController::ActionCheckResult override;
    };
}


#endif //SOPRAGAMELOGIC_INTERFERENCE_H
