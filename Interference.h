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
        virtual void execute() const = 0;

        /**
         * Checks if the interference is possible
         * @return
         */
        virtual bool isPossible() const = 0;

        /**
         * Gets the type of interference
         * @return
         */
        virtual auto getType() const -> gameModel::InterferenceType = 0;
    protected:

        std::shared_ptr<gameModel::Environment> env;
        std::shared_ptr<gameModel::Team> team;
        gameModel::InterferenceType type;

        /**
         * Checks if there are still enough fans left to execute the
         * @return
         */
        bool available() const;
    };

    class Teleport : public Interference {
    public:
        Teleport(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team,
                 std::shared_ptr<gameModel::Player> target);

        /**
         * Teleports target player to random free location on the field
         */
        void execute() const override;

        /**
         *
         * @return true if available
         */
        bool isPossible() const override;
        auto getType() const -> gameModel::InterferenceType override;

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
        void execute() const override;

        /**
         *
         * @return true if available and opponent target, false otherwise
         */
        bool isPossible() const override;
        auto getType() const -> gameModel::InterferenceType override;
    private:
        std::shared_ptr<gameModel::Player> target;
    };

    class Impulse : public Interference {
    public:
        Impulse(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team);

        /**
         * If a Keeper or Chaser holds the quaffle, quaffle is moved to a random free adjacent position
         */
        void execute() const override;

        /**
         *
         * @return true if available, false otherwise
         */
        bool isPossible() const override;
        auto getType() const -> gameModel::InterferenceType override;
    };

    class SnitchPush : public Interference {
    public:
        SnitchPush(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Team> team);

        /**
         * Snitch is moved to random free adjacent position
         */
        void execute() const override;

        /**
         *
         * @return true if available, false otherwise
         */
        bool isPossible() const override;
        auto getType() const -> gameModel::InterferenceType override;
    };
}


#endif //SOPRAGAMELOGIC_INTERFERENCE_H
