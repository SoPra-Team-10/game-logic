#include "SopraGameAction.h"

/**
 * main constructor for the action class
 * @tparam T
 * @param actor
 * @param target
 */
template<class T> Action<T>::Action(const std::shared_ptr<T> actor, const Position target) {
    this->actor = actor;
    this->target = target;
}

template<class T> std::vector<Action<T>> getAllPossibleActions(const std::shared_ptr<T> target, const std::shared_ptr<Environment> envi) {
    return std::vector<Action<T>>();
}
