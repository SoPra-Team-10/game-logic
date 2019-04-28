#ifndef SOPRAGAMELOGIC_SOPRAGAMEACTION_H
#define SOPRAGAMELOGIC_SOPRAGAMEACTION_H

#include <memory>
#include <vector>
#include "SopraGameModel.h"

/**
 * Type of the result of an action
 */
enum class ActionResult {
    success,
    impossible,
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
    Action<T>(const std::shared_ptr<T>, const Position);

    // objects
    const std::shared_ptr<T> actor;
    const Position target{};

    // functions
    virtual void execute(std::shared_ptr<Environment>) = 0;
    virtual double successProb() = 0;
    virtual ActionResult check(const std::shared_ptr<Environment>) = 0;
    virtual std::pair<Environment, double> ececuteAll(const std::shared_ptr<Environment>) = 0;
};


/**
 * Get all currently possible actions of a given actor in a given environment
 * @return a action vector
 */
template <class T> std::vector<Action<T>> getAllPossibleActions(const std::shared_ptr<T>, const std::shared_ptr<Environment>);


#endif //SOPRAGAMELOGIC_SOPRAGAMEACTION_H
