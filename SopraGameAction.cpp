#include "SopraGameAction.h"

/**
 * main constructor for the Action class.
 * @tparam T the actor type (Player or Ball).
 * @param actor the actor
 * @param target the target position
 */
template<class T> Action<T>::Action(const std::shared_ptr<T> actor, const Position target) {
    this->actor = actor;
    this->target = target;
}

/**
 * main constructor for the Shot class.
 * @param actor the acting player as shared pointer.
 * @param target the target position of the shot.
 */
Shot::Shot(const std::shared_ptr<Player> actor, const Position target) : Action<Player>(actor, target){

}

/**
 * execute the shot in a given environment (implementation of virtual function).
 * @param envi the environment in which the shot should be performed.
 */
void Shot::execute(std::shared_ptr<Environment> envi) {

}

/**
 * get the success probability of the shot (implementation of virtual function).
 * @return the success probability of the shot as double.
 */
double Shot::successProb() {
    return 0;
}

/**
 * check if the selected shot is possible (implementation of virtual function).
 * @param envi the selected environment.
 * @return the result of the check as ActionResult.
 */
ActionResult Shot::check(const std::shared_ptr<Environment> envi) {
    return ActionResult::foul;
}

/**
 * execute all given shots in a given environment (implementation of virtual function).
 * @param envi the selected environment.
 * @return the resulting environments an there probabilities as a pair.
 */
std::vector<std::pair<Environment, double>> executeAll(std::shared_ptr<Environment> envi) {
    std::vector<std::pair<Environment, double>> resultVect;

    return resultVect;
}

/**
 * main constructor for the Move class.
 * @tparam T the actor type (Player or Ball).
 * @param actor the acting player or ball as shared pointer.
 * @param target the target position of the move.
 */
template<class T> Move<T>::Move(std::shared_ptr<T> actor, Position target) : Action<T>(actor, target){

}

/**
 * execute the move in a given environment (implementation of virtual function).
 * @tparam T the actor type (Player or Ball).
 * @param envi the environment in which the shot should be performed.
 */
template<class T> void Move<T>::execute(std::shared_ptr<Environment> envi) {

}

/**
 * get the success probability of the move (implementation of virtual function). (implementation of virtual function).
 * @tparam T the actor type (Player or Ball).
 * @return the success probability of the move as double.
 */
template<class T> double Move<T>::successProb() {
    return 0;
}

/**
 * check if the selected move is possible  (implementation of virtual function)
 * @tparam T the actor type (Player or Ball).
 * @param envi the selected environment.
 * @return the result of the check as ActionResult.
 */
template<class T> ActionResult Move<T>::check(std::shared_ptr<Environment> envi) {
    return ActionResult::foul;
}

/**
 * execute all given move in a given environment (implementation of virtual function).
 * @tparam T the actor type (Player or Ball).
 * @param envi the selected environment.
 * @return the resulting environments an there probabilities as a pair.
 */
template<class T> std::vector<std::pair<Environment, double>> executeAll(std::shared_ptr<Environment> envi) {
    std::vector<std::pair<Environment, double>> resultVect;

    return resultVect;
}

/**
 * get all current actions which a player or a ball could perform in the environment.
 * @tparam T the actor type (Player or Ball).
 * @param actor the selected actor to test.
 * @param envi the selected actor to test.
 * @return a vector with all currently possible actions.
 */
template<class T> std::vector<Action<T>> getAllPossibleActions(const std::shared_ptr<T> actor, const std::shared_ptr<Environment> envi) {
    return std::vector<Action<T>>();
}