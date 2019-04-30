#ifndef SOPRAGAMELOGIC_SOPRAGAMEACTION_H
#define SOPRAGAMELOGIC_SOPRAGAMEACTION_H

#include <memory>
#include <cmath>
#include <vector>
#include "SopraGameModel.h"
#include "SopraGameController.h"

/**
 * Type of the result of an action
 */
enum class ActionResult {
    impossible,
    success,
    foul
};

/**
 * template class for a action in the game
 * @tparam T
 */
template <class T> class Action {
private:
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
    virtual void execute(std::shared_ptr<gameModel::Environment> envi) = 0;
    virtual auto successProb(std::shared_ptr<gameModel::Environment> envi) -> double = 0;
    virtual auto check(std::shared_ptr<gameModel::Environment> envi) -> ActionResult = 0;
    virtual auto executeAll(std::shared_ptr<gameModel::Environment> envi) ->
    std::vector<std::pair<gameModel::Environment, double>> = 0;
};

/**
 * class for a shot in the game which can only be executed by a player
 */
class Shot : Action<gameModel::Player> {
private:
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
    void execute(std::shared_ptr<gameModel::Environment> envi) override;
    /**
     * get the success probability of the shot (implementation of virtual function).
     * @return the success probability of the shot as double.
     */
    auto successProb(std::shared_ptr<gameModel::Environment> envi) -> double override;
    /**
     * check if the selected shot is possible (implementation of virtual function).
     * @param envi the selected environment.
     * @return the result of the check as ActionResult.
     */
    auto check(std::shared_ptr<gameModel::Environment> envi) -> ActionResult override;
    /**
     * execute all given shots in a given environment (implementation of virtual function).
     * @param envi the selected environment.
     * @return the resulting environments an there probabilities as a pair.
     */
    auto executeAll(std::shared_ptr<gameModel::Environment> envi) ->
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
    void execute(std::shared_ptr<gameModel::Environment> envi) override;
    /**
     * get the success probability of the move (implementation of virtual function). (implementation of virtual function).
     * @tparam T the actor type (Player or Ball).
     * @return the success probability of the move as double.
     */
    auto successProb(std::shared_ptr<gameModel::Environment> envi) -> double override;
    /**
    * check if the selected move is possible  (implementation of virtual function)
    * @tparam T the actor type (Player or Ball).
    * @param envi the selected environment.
    * @return the result of the check as ActionResult.
    */
    auto check(std::shared_ptr<gameModel::Environment> envi) -> ActionResult override;
    /**
     * execute all given move in a given environment (implementation of virtual function).
     * @tparam T the actor type (Player or Ball).
     * @param envi the selected environment.
     * @return the resulting environments an there probabilities as a pair.
     */
    auto executeAll(std::shared_ptr<gameModel::Environment> envi) ->
    std::vector<std::pair<gameModel::Environment, double>> override;
};

/**
 * Get all currently possible shots of a given player in a given environment.
 * @param actor actor the acting player as shared pointer.
 * @param envi the selected environment.
 * @return a shots vector
 */
auto getAllPossibleShots(std::shared_ptr<gameModel::Player> actor, std::shared_ptr<gameModel::Environment> envi);

/**
 * Get all currently possible moves of a given actor in a given environment
 * @tparam T the actor type (Player or Ball).
 * @param actor actor the acting player or ball as shared pointer.
 * @param envi the selected environment.
 * @return a action vector
 */
template <class T> auto getAllPossibleMoves(std::shared_ptr<T> actor, std::shared_ptr<gameModel::Environment> envi);


#endif //SOPRAGAMELOGIC_SOPRAGAMEACTION_H
