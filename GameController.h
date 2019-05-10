#ifndef GAMELOGIC_SOPRAGAMECONTROLLER_H
#define GAMELOGIC_SOPRAGAMECONTROLLER_H

#include <memory>
#include <vector>

#include "GameModel.h"
#include "Action.h"

namespace gameController {
    class Shot;
    class Move;
    class WrestQuaffle;

    enum class ExcessLength{
        None,
        Stage1,
        Stage2,
        Stage3
    };

    /**
     *
     * @tparam T Type of random number (int or double)
     * @param min lower boundary (inclusive)
     * @param max upper boundary (inclusive when integer is used, exclusive else)
     * @return
     */
     template <typename T>
    T rng(T min, T max);

    /**
     * Rolls the dice and determines whether an action takes place
     * @param actionProbability Probability of the action. Must be between 0 an 1
     * @return true if the action takes place
     */
    bool actionTriggered(double actionProbability);

    /**
     * get a vector containing all cells which are crossed by the vector between to cells. The
     * startPosition and endPosition are NOT included
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @return a vector containing all crossed cells.
     */
    auto getAllCrossedCells(const gameModel::Position &startPoint, const gameModel::Position &endPoint) ->
        std::vector<gameModel::Position>;

    /**
     * get the ditance between to cells on the game field.
     * @param startPoint position of the first cell.
     * @param endPoint position of the second cell.
     * @return the distance as integer.
     */
    auto getDistance(const gameModel::Position &startPoint, const gameModel::Position &endPoint) -> int;

    /**
     * Get all currently possible shots of a given player in a given environment.
     * @param actor actor the acting player as shared pointer.
     * @param envi the selected environment.
     * @return a shots vector
     */
    auto getAllPossibleShots(std::shared_ptr<gameModel::Player> actor, const gameModel::Environment &envi) ->
        std::vector<Shot>;

    /**
     * Get all currently possible moves of a given actor in a given environment
     * @tparam T the actor type (Player or Ball).
     * @param actor actor the acting player or ball as shared pointer.
     * @param envi the selected environment.
     * @return a action vector
     */
    auto getAllPossibleMoves(std::shared_ptr<gameModel::Player> actor, const gameModel::Environment &envi) ->
        std::vector<Move>;

    /**
     * moves the specified game object to an adjacent position according to the game rules
     * @param object to be moved
     * @param env the environment to operate on
     */
    void moveToAdjacent(const std::shared_ptr<gameModel::Object> &object, const std::shared_ptr<gameModel::Environment> &env);

    void moveQuaffelAfterGoal(const std::shared_ptr<gameModel::Environment> &env);


    template <>
    double rng(double min, double max);

    template <>
    int rng(int min, int max);

    template <typename T>
    T rng(T min, T mx){
        static_assert("Only double or int are supported");
        return min;
    }

    /**
     * make the decision if a player will be punished or not after a foul.df
     * @param foul the foul type.
     * @param gameConf the current game configuration.
     * @return true if player shall be punished, else false.
     */
    auto refereeDecision(const gameModel::Foul &foul, const gameModel::Config &gameConf) -> bool;

    /**
     * move the selected bludger according to the game roles.
     * @return the player the bludger attempted to knock out
     */
    auto moveBludger(std::shared_ptr<gameModel::Bludger> &bludger, std::shared_ptr<gameModel::Environment> &env) ->
        std::optional<std::shared_ptr<gameModel::Player>>;

    /**
     * check if a player can perform a shot or wrest quaffle.
     */
    bool playerCanPerformAction(const std::shared_ptr<const gameModel::Player> &player,
                                const std::shared_ptr<const gameModel::Environment> &env);

    /**
     * make the move for the Snitch as in the Rules given
     */
    void moveSnitch(std::shared_ptr<gameModel::Snitch> &snitch, std::shared_ptr<gameModel::Environment> &env, ExcessLength excessLength);
}



#endif //GAMELOGIC_SOPRAGAMECONTROLLER_H
