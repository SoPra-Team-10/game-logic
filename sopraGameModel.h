#ifndef GAMELOGIC_SOPRAGAMEMODEL_H
#define GAMELOGIC_SOPRAGAMEMODEL_H

#include <string>
#include <array>
#include <map>

namespace gameModel{

/**
 * Probabilities for detecting a foul
 */
    struct FoulDetectionProbs{
        double blockGoal,
                chargeGoal, multipleOffence,
                ramming, blockSnitch, teleport,
                rangedAttack, impulse, snitchPush;
    };

/**
 * All different kinds of time limits and timeouts
 */
    struct Timeouts{
        unsigned int playerTurn,
                fanTurn, playerPhase,
                fanPhase, ballPhase;
    };

/**
 * Probabilities for standard gameplay
 */
    struct GameDynamicsProbs{
        double throwSuccess,
                knockOut, foolAway,
                catchSnitch, catchQuaffle,
                wrestQuaffle;
    };

/**
 * This struct represents a 2D-Position on the Gamefield
 */
    struct Position{
        int x;
        int y;
    };

/**
 * Types of the playing field's cells
 */
    enum class Cell{
        GoalLeft, ///< Goal cell
        GoalRight,
        Standard, ///< Cell with no specialties
        Centre, ///< Cells belonging to the fields centre area
        RestrictedLeft, ///< Restricted area where only one attacker at a time is allowed
        RestrictedRight,
        OutOfBounds ///< Cells not belonging to the game field
    };

/**
 * For unnecessary gender fights
 */
    enum class Gender{
        Male,
        Female
    };

/**
 * Different broom types
 */
    enum class Broom{
        Thinderblast,
        Cleansweep_11,
        Comet_260,
        Nimbus_2001,
        Firebolt
    };

    enum class InterferenceType{
        RangedAttack,
        Teleport,
        Impulse,
        SnitchPush
    };

/**
 * Class containing metadata for a match
 */
    class Config{
    public:
        const unsigned int maxRounds;
        const Timeouts timeouts;
        const FoulDetectionProbs foulDetectionProbs;
        const GameDynamicsProbs gameDynamicsProbs;

        Config(unsigned int maxRounds, const Timeouts &timeouts, const FoulDetectionProbs &foulDetectionProbs,
               const GameDynamicsProbs &gameDynamicsProbs, std::map<Broom, double> extraTurnProbs);
        /**
         * Gets the probability of an extra turn with the specified Broom type
         * @param broom
         * @return
         */
        double getExtraTurnProb(Broom broom);
    private:
        std::map<Broom, double> extraTurnProbs;
    };

/**
 * Represents the playable characters
 */
    class Player{
    public:
        Position position = {};
        std::string name;
        Gender gender = Gender::Female;
        Broom broom = Broom::Cleansweep_11;

        Player() = default;
        Player(Position position, std::string  name, Gender gender, Broom broom);
    };

/**
 * Represents non playable ball-objects
 */
    class Ball{
    public:
        Position position = {};

        explicit Ball(Position position);
    };

/**
 * Represents available fans for a Team
 */
    class Fanblock{
    public:
        Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush);

        /**
         * gets the number of times the given fan might be used
         * @param fan the fan to check
         * @return number of left uses
         */
        int getUses(InterferenceType fan);

        /**
         * Bans a fan by decreasing the number of allowed uses by one
         * @param fan the fan to be banned
         * @throws std::invalid_argument if there are no more fans left to ban of the given type
         */
        void banFan(InterferenceType fan);
    private:
        std::map<InterferenceType, int> fans;

    };

    class Chaser : public Player{
    public:
        Chaser(Position position, std::string name, Gender gender, Broom broom);
    };

    class Keeper : public Player{
    public:
        Keeper(Position position, std::string name, Gender gender, Broom broom);
    };

    class Seeker : public Player{
    public:
        Seeker(Position position, std::string name, Gender gender, Broom broom);
    };

    class Beater : public Player{
    public:
        Beater(Position position, std::string name, Gender gender, Broom broom);
    };

    class Quaffle : public Ball{
    public:
        explicit Quaffle(Position position);
    };

    class Bludger : public Ball{
    public:
        explicit Bludger(Position position);
    };

    class Snitch : public Ball{
    public:
        bool exists = false;

        explicit Snitch(Position position);
    };

/**
 * Represents a Team
 */
    class Team{
    public:
        Seeker seeker;
        Keeper keeper;
        std::array<Beater, 2> beaters;
        std::array<Chaser, 3> chasers;
        const std::string name;
        const std::string colorMain;
        const std::string colorSecondary;
        int score{};
        Fanblock fanblock;

        Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
             std::string  name, std::string  colorMain, std::string  colorSecondary,
             const Fanblock &fanblock);
    };

/**
 * Represents a game state
 */
    class Environment{
    public:
        Team team1, team2;
        Quaffle quaffle;
        Snitch snitch;
        std::array<Bludger, 2> bludgers;

        Environment(Team team1, Team team2, Quaffle quaffle, Snitch snitch,
                    std::array<Bludger, 2> bludgers);

        /**
         * Gets the type of the cell at position (x,y)
         * @param x xPosition from left, 0 based
         * @param y yPosition from bottom, 0based
         * @return The corresponding Cell
         */
        static Cell getCell(int x, int y);

        /**
         * See overloaded function above
         * @param position
         * @return
         */
        static Cell getCell(Position position);
    };
}


#endif //GAMELOGIC_SOPRAGAMEMODEL_H