#ifndef GAMELOGIC_SOPRAGAMECONTROLLER_H
#define GAMELOGIC_SOPRAGAMECONTROLLER_H

#include <memory>
#include <vector>

#include "GameModel.h"
#include "Action.h"

namespace gameController {

    constexpr int GOAL_POINTS = 10;
    constexpr int SNITCH_POINTS = 30;

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
     * Enum for possible actions
     */
    enum class ActionType {
        Move, ///>Move
        Throw, ///>Throw
        Wrest ///>Wrest
    };

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
     * @param env the selected environment.
     * @param minSuccessProb minimal probability for the shot to succeed
     * @return a shots vector
     */
    auto getAllPossibleShots(const std::shared_ptr<gameModel::Player> &actor,
            const std::shared_ptr<gameModel::Environment> &env, double minSuccessProb) ->
        std::vector<Shot>;

    /**
     * Get all currently possible shots of a given player in a given environment. Optimized for QauffleThrows: Only throws on goals
     * and team mates are calculated
     * @param actor actor the acting player as shared pointer.
     * @param env the selected environment.
     * @return a shots vector
     */
    auto getAllPossibleShots(const std::shared_ptr<gameModel::Player> &actor,
            const std::shared_ptr<gameModel::Environment> &env) ->
            std::vector<Shot>;

    /**
     * Get all currently possible moves of a given actor in a given environment
     * @param actor actor the acting player
     * @param envi the selected environment.
     * @return a action vector
     */
    auto getAllPossibleMoves(const std::shared_ptr<gameModel::Player> &player, const std::shared_ptr<gameModel::Environment> &env) ->
        std::vector<Move>;

    /**
     * moves the specified game object to an adjacent position according to the game rules
     * @param object to be moved
     * @param env the environment to operate on
     */
    void moveToAdjacent(const std::shared_ptr<gameModel::Object> &object, const std::shared_ptr<gameModel::Environment> &env);

    void moveQuaffelAfterGoal(const std::shared_ptr<gameModel::Environment> &env);

    /**
     * generate a random number
     * @param min lower boundary (inclusive)
     * @param max upper boundary (exclusive)
     * @return a random double number between min and max
     */
    double rng(double min, double max);

    /**
     * generate a random number
     * @param min lower boundary (inclusive)
     * @param max upper boundary (inclusive)
     * @return a random integer number between min and max
     */
    int rng(int min, int max);

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
     * @param player the player
     * @param env the environment
     * @return true if possible, else false
     */
    bool playerCanPerformAction(const std::shared_ptr<const gameModel::Player> &player,
                                const std::shared_ptr<const gameModel::Environment> &env);

    /**
     * gets the actiontype of the action a player can perform with a ball
     * @param player the player
     * @param env the envirnoment
     * @return optinal with action type
     */
    auto getPossibleBallActionType(const std::shared_ptr<const gameModel::Player> &player,
                                   const std::shared_ptr<const gameModel::Environment> &env) ->
                                   std::optional<ActionType>;

    /**
     * make the move for the Snitch as in the Rules given
     * @return true if the snitch was caught after its move, false otherwise
     */
    bool moveSnitch(std::shared_ptr<gameModel::Snitch> &snitch, std::shared_ptr<gameModel::Environment> &env, ExcessLength excessLength);

    /**
     * This Method places the Snitch belong to the Rules. The Method shuld be called in the 13th Round of the Game
     */
    void spawnSnitch(std::shared_ptr<gameModel::Environment>& env);
}



#endif //GAMELOGIC_SOPRAGAMECONTROLLER_H
