#ifndef SOPRAGAMELOGIC_SOPRAGAMEACTION_H
#define SOPRAGAMELOGIC_SOPRAGAMEACTION_H

#include <memory>
#include <cmath>
#include <vector>
#include "SopraGameModel.h"
#include "SopraGameController.h"

namespace gameController{

    /**
     * Type of the result of an action
     */
    enum class ActionResult {
        Impossible,
        Success,
        Foul
    };

    /**
     * template class for a action in the game
     * @tparam T
     */
    template <class T> class Action {
    public:

        // constructors
        /**
         * default constructor for the Action class.
         */
        Action() = default;
        /**
         * main constructor for the Action class.
         * @tparam T the actor type (Player or Ball).
         * @param actor the actor
         * @param target the target position
         */
        Action(std::shared_ptr<T> actor, gameModel::Position target);

        // objects
        std::shared_ptr<T> actor;
        gameModel::Position target{};

        // functions
        virtual void execute(gameModel::Environment &envi) const = 0;
        virtual auto successProb(const gameModel::Environment &envi) const -> double = 0;
        virtual auto check(const gameModel::Environment &envi) const -> ActionResult = 0;
        virtual auto executeAll(const gameModel::Environment &envi) const ->
        std::vector<std::pair<gameModel::Environment, double>> = 0;
    };

    /**
     * class for a shot in the game which can only be executed by a player
     */
    class Shot : public Action<gameModel::Player> {
    private:

        /**
         * gets all cells along the flightpath which are occupied by opponent players (ordered in flight direction)
         * @param env
         * @return
         */
        auto getInterceptionPositions(const gameModel::Environment &env) const -> std::vector<gameModel::Position>;

        /**
         * roll the dice to get the position where the ball will land
         * @param envi
         * @return
         */
        auto rollTheDiceForLandingCell(std::shared_ptr<gameModel::Environment> envi) const -> gameModel::Position;

    public:

        // constructors
        /**
         * default constructor for the Shot class.
         */
        Shot() = default;
        /**
         * main constructor for the Shot class.
         * @param actor the acting player as shared pointer.
         * @param target the target position of the shot.
         */
        Shot(std::shared_ptr<gameModel::Player> actor, gameModel::Position target);

        // functions
        /**
        * execute the shot in a given environment (implementation of virtual function).
        * @param envi the environment in which the shot should be performed.
        */
        void execute(gameModel::Environment &envi) const override;
        /**
         * get the success probability of the shot (implementation of virtual function).
         * @return the success probability of the shot as double.
         */
        auto successProb(const gameModel::Environment &envi) const -> double override;
        /**
         * check if the selected shot is possible (implementation of virtual function).
         * @param envi the selected environment.
         * @return the result of the check as ActionResult.
         */
        auto check(const gameModel::Environment &envi) const -> ActionResult override;
        /**
         * execute all given shots in a given environment (implementation of virtual function).
         * @param envi the selected environment.
         * @return the resulting environments an there probabilities as a pair.
         */
        auto executeAll(const gameModel::Environment &envi) const ->
        std::vector<std::pair<gameModel::Environment, double>> override;

    };

    /**
     * clas for a move in the game which can be executed by a player or a ball
     * @tparam T
     */
    template <class T> class Move : Action<T> {
    private:
    public:

        // constructors
        /**
         * default constructor for the Move Class.
         */
        Move() = default;
        /**
         * main constructor for the Move class.
         * @tparam T the actor type (Player or Ball).
         * @param actor the acting player or ball as shared pointer.
         * @param target the target position of the move.
         */
        Move(std::shared_ptr<T> actor, gameModel::Position target);

        // functions
        /**
         * execute the move in a given environment (implementation of virtual function).
         * @tparam T the actor type (Player or Ball).
         * @param envi the environment in which the shot should be performed.
         */
        void execute(gameModel::Environment &envi) const override;
        /**
         * get the success probability of the move (implementation of virtual function). (implementation of virtual function).
         * @tparam T the actor type (Player or Ball).
         * @return the success probability of the move as double.
         */
        auto successProb(const gameModel::Environment &envi) const -> double override;
        /**
        * check if the selected move is possible  (implementation of virtual function)
        * @tparam T the actor type (Player or Ball).
        * @param envi the selected environment.
        * @return the result of the check as ActionResult.
        */
        auto check(const gameModel::Environment &envi) const -> ActionResult override;
        /**
         * execute all given move in a given environment (implementation of virtual function).
         * @tparam T the actor type (Player or Ball).
         * @param envi the selected environment.
         * @return the resulting environments an there probabilities as a pair.
         */
        auto executeAll(const gameModel::Environment &envi) const ->
        std::vector<std::pair<gameModel::Environment, double>> override;


    };
}




#endif //SOPRAGAMELOGIC_SOPRAGAMEACTION_H
