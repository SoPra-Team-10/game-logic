#ifndef GAMELOGIC_SOPRAGAMEMODEL_H
#define GAMELOGIC_SOPRAGAMEMODEL_H

#include <string>
#include <array>
#include <map>
#include <vector>
#include <memory>
#include <deque>
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
                rangedAttack, impulse, snitchPush/*, blockCell*/;
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
                knockOut,
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
        Vector(const Position &p1, const Position &p2);

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
        BlockGoal,
        ChargeGoal,
        MultipleOffence,
        Ramming,
        BlockSnitch
    };

    enum class InterferenceType{
        RangedAttack,
        Teleport,
        Impulse,
        SnitchPush/*,
        BlockCell*/
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

    /**
     * Base class for game objects like a ball or a player.
     * */
    class Object{
    public:
        Object() = default;
        Object(const Position &position, communication::messages::types::EntityId id);

        Position position = {};
        const communication::messages::types::EntityId id{};

        virtual ~Object() = default;
    };

    /**
     * Represents the playable characters
     */
    class Player : public Object{
    public:
        std::string name;
        communication::messages::types::Sex gender = {};
        communication::messages::types::Broom broom = {};
        bool isFined = false;
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
     * Represents the cube of shit that can be placed on a cell by a wombat.
     */
    class CubeOfShit : public Object {
    public:
        int round;
        CubeOfShit(const Position &position, communication::messages::types::EntityId id, int round);

        virtual ~CubeOfShit() = default;
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
        int getUses(InterferenceType fan) const ;

        int getUses(communication::messages::types::FanType fan) const;

        int getBannedCount(InterferenceType fan) const;
        int getBannedCount(communication::messages::types::FanType fan) const;

        /**
         * Bans a fan by decreasing the number of allowed uses by one
         * @param fan the fan to be banned
         * @throws std::invalid_argument if there are no more fans left to ban of the given type
         */
        void banFan(InterferenceType fan);
        void banFan(communication::messages::types::FanType fan);

        static auto fanToInterference(communication::messages::types::FanType fanType) -> InterferenceType;
        static auto interferenceToFan(InterferenceType type) -> communication::messages::types::FanType;
    private:
        std::map<InterferenceType, int> currFans;
        std::map<InterferenceType, const int> initialFans;

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
        explicit Bludger(communication::messages::types::EntityId id);
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
        std::shared_ptr<Seeker> seeker;
        std::shared_ptr<Keeper> keeper;
        std::array<std::shared_ptr<Beater>, 2> beaters;
        std::array<std::shared_ptr<Chaser>, 3> chasers;
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
        bool hasMember(const std::shared_ptr<Player>& player) const;

        /**
         * gets the number of banned players in the team
         */
        int numberOfBannedMembers() const;

        /**
         * Get player by server entity ids
         * @param id
         * @return
         */
        auto getPlayerByID(communication::messages::types::EntityId id) const -> std::optional<std::shared_ptr<Player>>;
    };

    /**
     * Represents a game state
     */
    class Environment{
    public:
        Config config;
        std::shared_ptr<Team> team1, team2;
        std::shared_ptr<Quaffle> quaffle;
        std::shared_ptr<Snitch> snitch;
        std::array<std::shared_ptr<Bludger>, 2> bludgers;
        std::deque<std::shared_ptr<CubeOfShit>> cubesOfShit;

        /**
         * Constructs an Environment from server config types
         * @param matchConfig
         * @param teamConfig
         * @param teamFormation
         */
        Environment(communication::messages::broadcast::MatchConfig matchConfig, const communication::messages::request::TeamConfig& teamConfig1,
                const communication::messages::request::TeamConfig& teamConfig2, communication::messages::request::TeamFormation teamFormation1,
                communication::messages::request::TeamFormation teamFormation2);

        /**
         * Automatically places all balls at the correct location
         * @param config
         * @param team1
         * @param team2
         */
        Environment(Config config, std::shared_ptr<Team> team1, std::shared_ptr<Team> team2);

        Environment(Config config, std::shared_ptr<Team> team1, std::shared_ptr<Team> team2, std::shared_ptr<Quaffle> quaffle,
                std::shared_ptr<Snitch> snitch, std::array<std::shared_ptr<Bludger>, 2> bludgers);

        /**
         * tests if two players are in the same team.
         * @param p1 player 1.
         * @param p2 player 2.
         * @return if the players are in the same team true, else false.
         */
        auto arePlayerInSameTeam(const std::shared_ptr<Player>& p1, const std::shared_ptr<Player>& p2) const -> bool;

        /**
         * checks if a player is in the own restricted zone.
         * @param player the player.
         * @return true if player is in own restricted zone, else false;
         */
        auto isPlayerInOwnRestrictedZone(const std::shared_ptr<Player>& player) const -> bool;

        /**
         * checks if a player is in the opponent restricted zone.
         * @param player the player.
         * @return true if player is in opponent restricted zone, else false;
         */
        auto isPlayerInOpponentRestrictedZone(const std::shared_ptr<Player>& player) const  -> bool;

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
        static Cell getCell(const Position &position);

        /**
         * Gets all valid Position surrounding the given position
         * @param position
         * @return
         */
        static auto getSurroundingPositions(const Position &position) -> std::vector<Position>;

        /**
         * Gets all goal cells in left half of the game field.
         * @return
         */
        static auto getGoalsLeft() -> std::array<Position, 3>;

        /**
         * Gets all goal cells in right half of the game field.
         * @return
         */
        static auto getGoalsRight() -> std::array<Position, 3>;

        /**
         * Checks if a given cell is a goal cell.
         * @param pos
         * @return
         */
        static auto isGoalCell(const Position &pos) -> bool;


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
        auto getTeamMates(const std::shared_ptr<Player>& player) const -> std::array<std::shared_ptr<Player>, 6>;

        /**
         * Gets all players from the opponent team of the given player
         * @param player
         * @return
         */
        auto getOpponents(const std::shared_ptr<Player>& player) const -> std::array<std::shared_ptr<Player>, 7>;

        /**
         * Determines whether the given Position is occupied by a Player
         * @param position the position to be checked
         * @return true if occupied, false otherwise
         */
        bool cellIsFree(const Position &position) const;

        bool cellIsFreeFromObject(const Position &position) const;

        /**
         * get all Positions around a given position where no other player is on. If all surrounding
         * cells are blocked the search window is enlarged until a free cell is found
         * @param position the position to be checked
         * @return
         */
        auto getAllPlayerFreeCellsAround(const Position &position) const -> std::vector<Position>;

        /**
         * Returns player object at the specified position if one exists
         * @return
         */
        auto getPlayer(const Position &position) const -> std::optional<std::shared_ptr<Player>>;

        /**
         * gets player by server entity id
         * @param id
         * @throws runtime_error when player cannot be found
         * @return
         */
        auto getPlayerById(communication::messages::types::EntityId id) const -> std::shared_ptr<Player>;

        /**
         * get the corresponding team of a player.
         * @param player the selected player.
         * @return the team of a player.
         */
        auto getTeam(const std::shared_ptr<Player>& player) const -> std::shared_ptr<Team>;

        /**
         * Gets all Positions which are not out of bounds
         * @return
         */
        static auto getAllValidCells() -> std::array<Position, 193>;

        /**
         * Gets all valid cells not occupied by players
         * @return
         */
        auto getAllFreeCells() -> std::vector<Position>;

        /**
         * place a player on random free cell in his half of the game field.
         * @param player
          */
        void placePlayerOnRandomFreeCell(const std::shared_ptr<Player>& player);

        /**
         * Get the corresponding ball object to an given id.
         * @param id the id of the ball.
         * @return the corresponding ball object.
         */
        auto getBallByID(const communication::messages::types::EntityId &id) const -> std::shared_ptr<Ball>;

        /**
         * Removes all the cubes of shit which were corrently on the game field.
         */
        void removeDeprecatedShit(int currentRound);

        void removeShitOnCell(const Position &position);

        auto isShitOnCell(const Position &position) const -> bool;
    };
}


#endif //GAMELOGIC_SOPRAGAMEMODEL_H