#ifndef GAMELOGIC_SOPRAGAMEMODEL_H
#define GAMELOGIC_SOPRAGAMEMODEL_H

#include <string>
#include <array>
#include <map>
#include <vector>
#include <memory>
#include <SopraMessages/types.hpp>
#include <SopraMessages/MatchConfig.hpp>
#include <SopraMessages/TeamConfig.hpp>
#include <SopraMessages/TeamFormation.hpp>

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
        int playerTurn,
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
    struct Position {
        int x;
        int y;

        Position() = default;
        Position(int x, int y);
        bool operator==(const Position &other) const;
        bool operator!=(const Position &other) const;
    };

    class Vector {
    public:

        // constructors
        Vector() = default;
        /**
         * main constructor for the vector.
         * @param x x component of the vector.
         * @param y y component of the vector.
         */
        Vector(double x, double y);

        // objects
        double x;   ///< x component of the vector.
        double y;   ///< y component of the vector.

        // methods
        /**
         * get the euclidean norm of the vector.
         * @return the euclidean norm as double.
         */
        double abs() const;
        /**
         * normalize the vector.
         */
        void normalize();

        // operators
        bool operator==(const Vector &v) const;
        Vector operator*(const double &c) const;
        Vector operator+(const Vector &v) const;
        Position operator+(const Position &p) const;
    };

    /**
     * Types of the playing field's cells
     */
    enum class Cell{
        GoalLeft, ///< Goal cell that belongs to team 1
        GoalRight, ///< Goal cell that belongs to team 2
        Standard, ///< Cell with no specialties
        Centre, ///< Cells belonging to the fields centre area
        RestrictedLeft, ///< Restricted area that belongs to team 1 where only one attacker at a time is allowed
        RestrictedRight, ///< Restricted area that belongs to team 2 where only one attacker at a time is allowed
        OutOfBounds ///< Cells not belonging to the game field
    };

    /**
     * Types of fouls.
     */
    enum class Foul {
        None,
        Flacken,
        Nachtarocken,
        Stutschen,
        Keilen,
        Schnatzeln
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

        Config(const communication::messages::broadcast::MatchConfig &config);
        Config(unsigned int maxRounds, const Timeouts &timeouts, const FoulDetectionProbs &foulDetectionProbs,
               const GameDynamicsProbs &gameDynamicsProbs, std::map<communication::messages::types::Broom, double> extraTurnProbs);
        /**
         * Gets the probability of an extra turn with the specified Broom type
         * @param broom
         * @return
         */
        double getExtraTurnProb(communication::messages::types::Broom broom) const;
    private:
        std::map<communication::messages::types::Broom, double> extraTurnProbs;
    };

    class Object{
    public:
        Object() = default;
        Object(const Position &position, communication::messages::types::EntityId id);

        Position position = {};
        const communication::messages::types::EntityId id{};
    };

    /**
     * Represents the playable characters
     */
    class Player : public Object{
    public:
        std::string name;
        communication::messages::types::Sex gender = {};
        communication::messages::types::Broom broom = {};
        bool knockedOut = false;

        Player() = default;
        Player(Position position, std::string  name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
        bool operator==(const Player &other) const;
        bool operator!=(const Player &other) const;

        virtual ~Player() = default;
    };

    /**
     * Represents non playable ball-objects
     */
    class Ball : public Object{
    public:
        Ball(Position position, communication::messages::types::EntityId id);

        virtual ~Ball() = default;
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
        Chaser(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    class Keeper : public Player{
    public:
        Keeper(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    class Seeker : public Player{
    public:
        Seeker(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    class Beater : public Player{
    public:
        Beater(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    class Quaffle : public Ball{
    public:
        /**
         * Places Quaffle in the centre of the field
         */
        Quaffle();
        explicit Quaffle(Position position);
    };

    class Bludger : public Ball{
    public:
        /**
         * Places Bludger in the centre of the field
         */
        Bludger(communication::messages::types::EntityId id);
        Bludger(Position position, communication::messages::types::EntityId id);
    };

    class Snitch : public Ball{
    public:
        bool exists = false;

        /**
         * Places Snitch on random position on the field and makes it non existent
         */
        Snitch();
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

        /**
         * Constructs a Team from server config types
         * @param leftTeam select if team si on left or right side
         */
        Team(const communication::messages::request::TeamConfig& tConf, communication::messages::request::TeamFormation tForm, bool leftTeam);

        Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
             std::string  name, std::string  colorMain, std::string  colorSecondary,
             Fanblock fanblock);

        /**
         * gets all Players of the team
         * @return
         */
        auto getAllPlayers() const -> std::array<std::shared_ptr<Player>, 7>;

        /**
         * Determins wether a given player is a member of the team
         * @param player
         * @return true if player is a member of the team. false otherwise
         */
        bool hasMember(const Player &player) const;

    };

    /**
     * Represents a game state
     */
    class Environment{
    public:
        Config config;
        Team team1, team2;
        Quaffle quaffle;
        Snitch snitch;
        std::array<Bludger, 2> bludgers;

        /**
         * Constructs an Environment from server config types
         * @param matchConfig
         * @param teamConfig
         * @param teamFormation
         */
        Environment(communication::messages::broadcast::MatchConfig matchConfig, const communication::messages::request::TeamConfig& teamConfig,
                communication::messages::request::TeamFormation teamFormation);

        /**
         * Automatically places all balls at the correct location
         * @param config
         * @param team1
         * @param team2
         */
        Environment(Config config, Team team1, Team team2);

        Environment(Config config, Team team1, Team team2, Quaffle quaffle,
                Snitch snitch, std::array<Bludger, 2> bludgers);

        /**
         * tests if two players are in the same team.
         * @param p1 player 1.
         * @param p2 player 2.
         * @return if the players are in the same team true, else false.
         */
        auto arePlayerInSameTeam(const Player &p1, const Player & p2) const -> bool;

        /**
         * checks if a player is in the own restricted zone.
         * @param player the player.
         * @return true if player is in own restricted zone, else false;
         */
        auto isPlayerInOwnRestrictedZone(const Player &player) const -> bool;

        /**
         * checks if a player is in the opponent restricted zone.
         * @param player the player.
         * @return true if player is in opponent restricted zone, else false;
         */
        auto isPlayerInOpponentRestrictedZone(const Player &player) const  -> bool;

        /**
         * Gets the type of the cell at position (x,y)
         * @param x xPosition from left, 0 based
         * @param y yPosition from bottom, 0 based
         * @return The corresponding Cell
         */
        static Cell getCell(int x, int y);

        /**
         * See overloaded function above
         * @param position
         * @return
         */
        static Cell getCell(Position position);

        /**
         * Gets all players on the field
         * @return
         */
        auto getAllPlayers() const -> std::array<std::shared_ptr<Player>, 14>;

        /**
         * Gets all players in the same team as the given player, themselves excluded
         * @param player
         * @return
         */
        auto getTeamMates(const Player &player) const -> std::array<std::shared_ptr<Player>, 6>;

        /**
         * Gets all players from the opponent team of the given player
         * @param player
         * @return
         */
        auto getOpponents(const Player &player) const -> std::array<std::shared_ptr<Player>, 7>;

        /**
         * Determines whether the given Position is occupied by a Player
         * @param position the position to be checked
         * @return true if occupied, false otherwise
         */
        bool cellIsFree(const Position &position) const;

        /**
         * get all Positions around a given position where no odther player is on.
         * @param position the position to be checked
         * @return
         */
        auto getAllPlayerFreeCellsAround(const Position &position) const -> std::vector<Position>;

        /**
         * Returns player object at the specified position if one exists
         * @return
         */
        auto getPlayer(Position) const -> std::optional<std::shared_ptr<Player>>;

        /**
         * Gets all Positions which are not out of bounds
         * @return
         */
        static auto getAllValidCells() -> std::array<Position, 193>;

        /**
         * Gets all valid cells not occupied by players
         * @return
         */
        auto getAllFreeCells() -> std::array<Position, 179>;
    };
}


#endif //GAMELOGIC_SOPRAGAMEMODEL_H