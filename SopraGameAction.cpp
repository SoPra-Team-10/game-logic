#include "SopraGameAction.h"

template<class T> Action<T>::Action(const std::shared_ptr<T> actor, const gameModel::Position target) {

    this->actor = actor;
    this->target = target;
}

auto getAllPossibleShots(std::shared_ptr<gameModel::Player>, std::shared_ptr<gameModel::Environment>) {

    std::vector<Shot> resultVect;

    return resultVect;
}

template <class T> auto getAllPossibleMoves(std::shared_ptr<T>, std::shared_ptr<gameModel::Environment>) {

    std::vector<Move<T>> resultVect;

    return resultVect;
}

Shot::Shot(const std::shared_ptr<gameModel::Player> actor, const gameModel::Position target) : Action<gameModel::Player>(actor, target) {

}

void Shot::execute(std::shared_ptr<gameModel::Environment> envi) {

    if (this->check(envi) == ActionResult::impossible) return;
}

auto Shot::successProb() -> double {

    return 0;
}

auto Shot::check(const std::shared_ptr<gameModel::Environment> envi) -> ActionResult {

    return ActionResult::foul;
}

auto Shot::executeAll(std::shared_ptr<gameModel::Environment> envi) -> std::vector<std::pair<gameModel::Environment, double>> {

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

template<class T> Move<T>::Move(std::shared_ptr<T> actor, gameModel::Position target) : Action<T>(actor, target){

}

template<class T> void Move<T>::execute(std::shared_ptr<gameModel::Environment> envi) {
    if (this->check(envi) == ActionResult::impossible) return;
}

template<class T> auto Move<T>::successProb() -> double {
    return 0;
}

template<class T> auto Move<T>::check(std::shared_ptr<gameModel::Environment> envi) -> ActionResult {
    return ActionResult::foul;
}

template<class T> auto Move<T>::executeAll(std::shared_ptr<gameModel::Environment> envi) -> std::vector<std::pair<gameModel::Environment, double>> {

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