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
     * Basic types of actions
     */
    enum class ActionCheckResult {
        Impossible, ///< Action violates game mechanics
        Success, ///< Action is possible without risk of a foul
        Foul ///< Action may result in a foul
    };

    /**
     * Outcomes of a Shot
     */
    enum class ActionResult {
        Intercepted, ///<Quaffle is intercepted
        Miss, ///<Quaffle does not land on target position
        ScoreRight, ///<Throw resulted in a goal for the right team
        ScoreLeft, ///<Throw resulted in a goal for the left team
        ThrowSuccess, ///<Quaffle landed on target position
        Knockout, ///<Bludger knocked out a player
        SnitchCatch, ///Snitch is catched
        WrestQuaffel, ///>Quaffel was wrested
        FoolAway
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
        virtual auto execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>> = 0;
        virtual auto successProb() const -> double = 0;
        virtual auto check() const -> ActionCheckResult = 0;
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
    class Shot : public Action {
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
        auto execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>>;
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
        auto check() const -> ActionCheckResult override;
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

        /**
         * Checks if a goal was scored depending on the quaffles and actors current position
         * @return
         */
        auto goalCheck(const gameModel::Position &pos) const -> std::vector<ActionResult>;
    };

    /**
     * class for a the action of wresting the quaffel from anoter player.
     */
    class WrestQuaffle : public Action {
    public:

        // constructors
        /**
         * default constructor for the WrestQuaffle class.
         */
        WrestQuaffle() = default;

        /**
         * main constructor for the WrestQuaffle class.
         * @param env the environment to operate on
         * @param actor the acting player (only chaser) as shared pointer.
         * @param target the target position of the shot.
         */
        WrestQuaffle(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Chaser> actor,
                     gameModel::Position target);

        // functions
        /**
        * execute the shot in a given environment (implementation of virtual function).
        * @param envi the environment in which wresting of the quaffel should be performed.
        */
        auto execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>>;
        /**
         * get the success probability of the shot (implementation of virtual function).
         * @return the success probability of wresting the quaffel as double.
         */
        auto successProb() const -> double override;
        /**
         * check if the wresting the quaffel is possible (implementation of virtual function).
         * @param envi the selected environment.
         * @return the result of the check as ActionResult.
         */
        auto check() const -> ActionCheckResult override;
        /**
         * execute all wresting the quaffel actions in a given environment (implementation of virtual function).
         * @param envi the selected environment.
         * @return the resulting environments an there probabilities as a pair.
         */
        auto executeAll() const ->
        std::vector<std::pair<gameModel::Environment, double>> override;

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
        auto execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>> override;
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
        auto check() const -> ActionCheckResult override;
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
        auto checkForFoul() const -> std::vector<gameModel::Foul>;
    };

}

#endif //SOPRAGAMELOGIC_SOPRAGAMEACTION_H
