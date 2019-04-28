#ifndef SOPRAGAMELOGIC_SOPRAGAMEACTION_H
#define SOPRAGAMELOGIC_SOPRAGAMEACTION_H

#include <memory>
#include <vector>
#include "SopraGameModel.h"

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
    Action() = default;
    Action(std::shared_ptr<T>, Position);

    // objects
    std::shared_ptr<T> actor;
    Position target{};

    // functions
    virtual void execute(std::shared_ptr<Environment>) = 0;
    virtual double successProb() = 0;
    virtual ActionResult check(std::shared_ptr<Environment>) = 0;
    virtual std::vector<std::pair<Environment, double>> executeAll(std::shared_ptr<Environment>) = 0;
};

/**
 * class for a shot in the game which can only be executed by a player
 */
class Shot : Action<Player> {
private:
public:

    // constructors
    Shot() = default;
    Shot(std::shared_ptr<Player>, Position);

    // objects


    // functions
    void execute(std::shared_ptr<Environment>) override;
    double successProb() override;
    ActionResult check(std::shared_ptr<Environment>) override;
    std::vector<std::pair<Environment, double>> executeAll(std::shared_ptr<Environment>) override;
};

/**
 * clas for a move in the game which can be executed by a player or a ball
 * @tparam T
 */
template <class T> class Move : Action<T> {
private:
public:

    // constructors
    Move() = default;
    Move(std::shared_ptr<T>, Position);

    // objects


    // functions
    void execute(std::shared_ptr<Environment>) override;
    double successProb() override;
    ActionResult check(std::shared_ptr<Environment>) override;
    std::vector<std::pair<Environment, double>> executeAll(std::shared_ptr<Environment>) override;
};

/**
 * Get all currently possible shots of a given player in a given environment
 * @return a action vector
 */
std::vector<Shot> getAllPossibleShots(std::shared_ptr<Player>, std::shared_ptr<Environment>);

/**
 * Get all currently possible moves of a given actor in a given environment
 * @tparam T the actor type (Player or Ball).
 * @return a action vector
 */
template <class T> std::vector<Move<T>> getAllPossibleMoves(std::shared_ptr<T>, std::shared_ptr<Environment>);


#endif //SOPRAGAMELOGIC_SOPRAGAMEACTION_H
