#include "SopraGameAction.h"

/**
 * main constructor for the Action class.
 * @tparam T the actor type (Player or Ball).
 * @param actor the actor
 * @param target the target position
 */
template<class T> Action<T>::Action(const std::shared_ptr<T> actor, const gameModel::Position target) {

    this->actor = actor;
    this->target = target;
}

/**
 * main constructor for the Shot class.
 * @param actor the acting player as shared pointer.
 * @param target the target position of the shot.
 */
Shot::Shot(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) : Action<gameModel::Player>(actor, target){

}

/**
 * execute the shot in a given environment (implementation of virtual function).
 * @param envi the environment in which the shot should be performed.
 */
void Shot::execute(std::shared_ptr<gameModel::Environment> envi) {

    if (this->check(envi) == ActionResult::impossible) return;
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
ActionResult Shot::check(const std::shared_ptr<gameModel::Environment> envi) {

    return ActionResult::foul;
}

/**
 * execute all given shots in a given environment (implementation of virtual function).
 * @param envi the selected environment.
 * @return the resulting environments an there probabilities as a pair.
 */
std::vector<std::pair<gameModel::Environment, double>> Shot::executeAll(std::shared_ptr<gameModel::Environment> envi) {

    std::vector<std::pair<gameModel::Environment, double>> resultVect;
    std::vector<Shot> possibleShots = getAllPossibleShots(this->actor, envi);

    for (auto & possibleShot : possibleShots) {

        if (possibleShot.check(envi) == ActionResult::impossible) continue;

        gameModel::Environment testEnvi = *envi;
        std::shared_ptr<gameModel::Environment> testEnviPtr(new gameModel::Environment(testEnvi));

        possibleShot.execute(testEnviPtr);

        resultVect.emplace_back(std::pair<gameModel::Environment, double>(testEnvi, possibleShot.successProb()));
    }

    return resultVect;
}

/**
 * main constructor for the Move class.
 * @tparam T the actor type (Player or Ball).
 * @param actor the acting player or ball as shared pointer.
 * @param target the target position of the move.
 */
template<class T> Move<T>::Move(std::shared_ptr<T> actor, gameModel::Position target) : Action<T>(actor, target){

}

/**
 * execute the move in a given environment (implementation of virtual function).
 * @tparam T the actor type (Player or Ball).
 * @param envi the environment in which the shot should be performed.
 */
template<class T> void Move<T>::execute(std::shared_ptr<gameModel::Environment> envi) {
    if (this->check(envi) == ActionResult::impossible) return;
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
template<class T> ActionResult Move<T>::check(std::shared_ptr<gameModel::Environment> envi) {
    return ActionResult::foul;
}

/**
 * execute all given move in a given environment (implementation of virtual function).
 * @tparam T the actor type (Player or Ball).
 * @param envi the selected environment.
 * @return the resulting environments an there probabilities as a pair.
 */
template<class T> std::vector<std::pair<gameModel::Environment, double>> Move<T>::executeAll(std::shared_ptr<gameModel::Environment> envi) {

    std::vector<std::pair<gameModel::Environment, double>> resultVect;
    std::vector<Move<T>> possibleMoves = getAllPossibleMoves(this->actor, envi);

    for (auto & possibleShot : possibleMoves) {

        if (possibleShot.check(envi) == ActionResult::impossible) continue;

        gameModel::Environment testEnvi = *envi;
        std::shared_ptr<gameModel::Environment> testEnviPtr(new gameModel::Environment(testEnvi));

        possibleShot.execute(testEnviPtr);

        resultVect.emplace_back(std::pair<gameModel::Environment, double>(testEnvi, possibleShot.successProb()));
    }

    return resultVect;
}

/**
 * Get all currently possible shots of a given player in a given environment
 * @return
 */
std::vector<Shot> getAllPossibleShots(std::shared_ptr<gameModel::Player>, std::shared_ptr<gameModel::Environment>) {

    std::vector<Shot> resultVect;

    return resultVect;
}

/**
 * Get all currently possible moves of a given actor in a given environment
 * @tparam T the actor type (Player or Ball).
 * @return
 */
template <class T> std::vector<Move<T>> getAllPossibleMoves(std::shared_ptr<T>, std::shared_ptr<gameModel::Environment>) {

    std::vector<Move<T>> resultVect;

    return resultVect;
}