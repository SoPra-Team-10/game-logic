#ifndef SOPRAGAMELOGIC_SOPRAGAMEACTION_H
#define SOPRAGAMELOGIC_SOPRAGAMEACTION_H

#include <memory>
#include <cmath>
#include <vector>
#include "GameController.h"
#include "GameModel.h"

#define INSTANCE_OF(A, B) (std::dynamic_pointer_cast<B>(A))

namespace gameController{

    /**
     * Type of the result of an action
     */
    enum class ActionResult {
        Impossible,
        Success,
        Foul
    };

    class Action {
    public:

        // constructors
        /**
         * default constructor for the Action class.
         */
        Action() = default;
        /**
         * main constructor for the Action class.
         * @param env The environment to operate on
         * @param actor the actor
         * @param target the target position
         */
        Action(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
                gameModel::Position target);


        // functions
        virtual void execute() const = 0;
        virtual auto successProb() const -> double = 0;
        virtual auto check() const -> ActionResult = 0;
        virtual auto executeAll() const -> std::vector<std::pair<gameModel::Environment, double>> = 0;

    protected:
        // objects
        std::shared_ptr<gameModel::Player> actor;
        std::shared_ptr<gameModel::Environment> env;
        gameModel::Position target{};
    };

    /**
     * class for a shot in the game which can only be executed by a player
     */
    class Shot : public Action{
    public:

        // constructors
        /**
         * main constructor for the Shot class.
         * @param env the environment to operate on
         * @param actor the acting player as shared pointer.
         * @param ball The ball to be moved
         * @param target the target position of the shot.
         */
        Shot(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
                std::shared_ptr<gameModel::Ball> ball, gameModel::Position target);

        // functions
        /**
        * execute the shot in a given environment (implementation of virtual function).
        * @param envi the environment in which the shot should be performed.
        */
        void execute() const override;
        /**
         * get the success probability of the shot (implementation of virtual function).
         * @return the success probability of the shot as double.
         */
        auto successProb() const -> double override;
        /**
         * check if the selected shot is possible (implementation of virtual function).
         * @param envi the selected environment.
         * @return the result of the check as ActionResult.
         */
        auto check() const -> ActionResult override;
        /**
         * execute all given shots in a given environment (implementation of virtual function).
         * @param envi the selected environment.
         * @return the resulting environments an there probabilities as a pair.
         */
        auto executeAll() const ->
            std::vector<std::pair<gameModel::Environment, double>> override;
    private:

        std::shared_ptr<gameModel::Ball> ball;
        /**
         * gets all cells along the flightpath which are occupied by opponent players (ordered in flight direction)
         * @param env
         * @return
         */
        auto getInterceptionPositions() const -> std::vector<gameModel::Position>;

        /**
         * Returns a list with all possible positions the ball might land if NOT intercepted. The target position is NOT
         * included
         * @param env
         * @return
         */
        auto getAllLandingCells() const -> std::vector<gameModel::Position>;
    };

    /**
     * class for a move in the game which can be executed by a player or a ball
     */
    class Move : Action{
    public:

        // constructors
        /**
         * default constructor for the Move Class.
         */
        Move() = default;
        /**
         * main constructor for the Move class.
         * @param env the environment to operate on
         * @param actor the acting player or ball as shared pointer.
         * @param target the target position of the move.
         */
        Move(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
                gameModel::Position target);

        // functions
        /**
         * execute the move in a given environment (implementation of virtual function).
         * @param envi the environment in which the shot should be performed.
         */
        void execute() const override;
        /**
         * get the success probability of the move (implementation of virtual function). (implementation of virtual function).
         * @return the success probability of the move as double.
         */
        auto successProb() const -> double override;
        /**
        * check if the selected move is possible  (implementation of virtual function)
        * @param envi the selected environment.
        * @return the result of the check as ActionResult.
        */
        auto check() const -> ActionResult override;
        /**
         * execute all given move in a given environment (implementation of virtual function).
         * @param envi the selected environment.
         * @return the resulting environments an there probabilities as a pair.
         */
        auto executeAll() const ->
            std::vector<std::pair<gameModel::Environment, double>> override;

        /**
         * checks if the move is a foul.
         * @param envi the selected environment.
         * @return the type of foul.
         */
        auto checkForFoul() const -> gameModel::Foul;
    };

}

#endif //SOPRAGAMELOGIC_SOPRAGAMEACTION_H
