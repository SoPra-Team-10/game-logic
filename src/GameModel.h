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
#include <SopraMessages/json.hpp>

namespace gameModel{
    constexpr int FIELD_CENTRE_COL = 8;

    /**
     * Probabilities for detecting a foul
     */
    struct FoulDetectionProbs{
        bool operator==(const FoulDetectionProbs &rhs) const;

        bool operator!=(const FoulDetectionProbs &rhs) const;

        double blockGoal,
                chargeGoal, multipleOffence,
                ramming, blockSnitch, teleport,
                rangedAttack, impulse, snitchPush, blockCell;
    };

    /**
     * Probabilities for standard gameplay
     */
    struct GameDynamicsProbs{
        bool operator==(const GameDynamicsProbs &rhs) const;

        bool operator!=(const GameDynamicsProbs &rhs) const;

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

    /**
     * 2D vector
     */
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

        /**
         * @return makes the vector orthogonal to its old vector
         */
        Vector orthogonal() const;

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

    /**
     * Different types of fan interferences
     */
    enum class InterferenceType{
        RangedAttack,
        Teleport,
        Impulse,
        SnitchPush,
        BlockCell
    };

    enum class TeamSide : char {
        LEFT, RIGHT
    };

    /**
     * Class containing metadata for a match
     */
    class Config{
    public:

        Config() = default;
        Config(const communication::messages::broadcast::MatchConfig &config);
        Config(unsigned int maxRounds, const FoulDetectionProbs &foulDetectionProbs,
               const GameDynamicsProbs &gameDynamicsProbs, std::map<communication::messages::types::Broom, double> extraTurnProbs);
        /**
         * Gets the probability of an extra turn with the specified Broom type
         * @param broom
         * @return
         */
        double getExtraTurnProb(communication::messages::types::Broom broom) const;

        /**
         * Gets the probability that the given foul will be detected
         * @param foul
         * @return
         */
        double getFoulDetectionProb(Foul foul) const;

        /**
         * Gets the probability that the given interference will be detected
         * @param interference
         * @return
         */
        double getFoulDetectionProb(InterferenceType interference) const;

        /**
         * Getter
         * @return maximum number of rounds before overtime
         */
        unsigned int getMaxRounds() const;

        /**
         * Getter
         * @return game dynamics probabilities
         */
        const GameDynamicsProbs &getGameDynamicsProbs() const;

        /**
         * Friend method for serialization
         */
        friend void from_json(const nlohmann::json &, Config &);

        bool operator==(const Config &rhs) const;

        bool operator!=(const Config &rhs) const;

    private:
        unsigned int maxRounds;
        FoulDetectionProbs foulDetectionProbs;
        GameDynamicsProbs gameDynamicsProbs;
        std::map<communication::messages::types::Broom, double> extraTurnProbs;
    };

    /**
     * Base class for game objects like a ball or a player.
     */
    class Object {
    public:
        Object() = default;
        Object(const Position &position, communication::messages::types::EntityId id);

        Position position = {};

        virtual ~Object() = default;

        bool operator==(const Object &other) const;
        bool operator!=(const Object &other) const;

        /**
         * Getter
         * @return object id
         */
        communication::messages::types::EntityId getId() const;

        /**
         * Friend method for serialization
         */
        friend void from_json(const nlohmann::json &, Object &);

    protected:
        communication::messages::types::EntityId id{};
    };

    /**
     * Represents the playable characters
     */
    class Player : public Object{
    public:
        communication::messages::types::Broom broom = {};
        bool isFined = false;
        bool knockedOut = false;

        Player() = default;
        Player(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
        bool operator==(const Player &other) const;
        bool operator!=(const Player &other) const;

        virtual ~Player() = default;
    };

    /**
     * Represents non playable ball-objects
     */
    class Ball : public Object{
    public:
        Ball() = default;

        Ball(Position position, communication::messages::types::EntityId id);

        virtual ~Ball() = default;
    };

    /**
     * Represents the cube of shit that can be placed on a cell by a wombat.
     */
    class CubeOfShit : public Object {
    public:
        CubeOfShit() = default;
        explicit CubeOfShit(const Position &target);
        bool spawnedThisRound = true;

        bool operator==(const CubeOfShit &other) const;
        bool operator!=(const CubeOfShit &other) const;
    };

    /**
     * Represents available fans for a Team
     */
    class Fanblock{
    public:
        Fanblock() = default;
        Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush, int blockCell);

        /**
         * gets the number of times the given fan might be used
         * @param fan the fan to check
         * @return number of left uses
         */
        int getUses(InterferenceType fan) const ;

        /**
         * gets the number of times the given fan might be used
         * @param fan the fan to check
         * @return number of left uses
         */
        int getUses(communication::messages::types::FanType fan) const;

        /**
         * gets the number of banned fans of the given type
         * @param fan
         * @return
         */
        int getBannedCount(InterferenceType fan) const;

        /**
         * gets the number of banned fans of the given type
         * @param fan
         * @return
         */
        int getBannedCount(communication::messages::types::FanType fan) const;

        /**
         * Bans a fan by decreasing the number of allowed uses by one
         * @param fan the fan to be banned
         * @throws std::invalid_argument if there are no more fans left to ban of the given type
         */
        void banFan(InterferenceType fan);

        /**
         * Bans a fan by decreasing the number of allowed uses by one
         * @param fan the fan to be banned
         * @throws std::invalid_argument if there are no more fans left to ban of the given type
         */
        void banFan(communication::messages::types::FanType fan);

        friend void from_json(const nlohmann::json &, Fanblock &);

        bool operator==(const Fanblock &other) const;
        bool operator!=(const Fanblock &other) const;

    private:
        std::map<InterferenceType, int> currFans;
        std::map<InterferenceType, const int> initialFans;

    };

    /**
     * Chaser type player
     */
    class Chaser : public Player{
    public:
        Chaser() = default;
        Chaser(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    /**
     * Keeper type player
     */
    class Keeper : public Player{
    public:
        Keeper() = default;
        Keeper(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    /**
     * Seeker type Player
     */
    class Seeker : public Player{
    public:
        Seeker() = default;
        Seeker(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    /**
     * Beater type Player
     */
    class Beater : public Player{
    public:
        Beater() = default;
        Beater(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id);
    };

    /**
     * Quaffle type Ball
     */
    class Quaffle : public Ball{
    public:
        /**
         * Places Quaffle in the centre of the field
         */
        Quaffle();

        /**
         * Playes the Quaffle at the specified location
         * @param position Position where the Quaffle is constructed
         */
        explicit Quaffle(Position position);
    };

    /**
     * Bludger type Ball
     */
    class Bludger : public Ball{
    public:

        Bludger() = default;

        /**
         * Places Bludger in the centre of the field
         * @param id Id of the Bludger
         */
        explicit Bludger(communication::messages::types::EntityId id);

        /**
         * Places Bludger at the specified location
         * @param position Position where the Bludger is constructed
         * @param id Id of the Bludger
         */
        Bludger(Position position, communication::messages::types::EntityId id);
    };

    /**
     * Snitch type Ball
     */
    class Snitch : public Ball{
    public:
        bool exists = false;

        /**
         * Places Snitch on random position on the field and makes it non existent
         */
        Snitch();

        /**
         * Places Snitch at the specified location
         * @param position Position where the Snitch is constructed
         */
        explicit Snitch(Position position);

        bool operator==(const Snitch &other) const;
        bool operator!=(const Snitch &other) const;
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
        int score{};
        Fanblock fanblock;

        Team() = default;

        /**
         * Constructs a Team from server config types with initial score of 0
         * @param leftTeam select if team si on left or right side
         */
        Team(const communication::messages::request::TeamConfig& tConf, communication::messages::request::TeamFormation tForm, TeamSide side);

        /**
         * Ctor where all members can be specified
         * @param seeker Seeker Player
         * @param keeper Keeper Player
         * @param beaters Array of two Beater Players
         * @param chasers Array of three Chaser Players
         * @param score initial score of the team
         * @param fanblock fans of the team
         */
        Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
                int score, Fanblock fanblock, TeamSide side);

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
        bool hasMember(const std::shared_ptr<const Player>& player) const;

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

        /**
         * returns a deep copy of the current Team
         * @return
         */
        auto clone() const -> std::shared_ptr<Team>;

        /**
         * Getter
         * @return side of the team
         */
        TeamSide getSide() const;

        friend void from_json(const nlohmann::json &, Team &);

        bool operator==(const Team &other) const;
        bool operator!=(const Team &other) const;

    private:
        TeamSide side;
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
        std::deque<std::shared_ptr<CubeOfShit>> pileOfShit;

        Environment() = default;

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

        /**
         * Ctor where all members can be specified
         * @param config
         * @param team1
         * @param team2
         * @param quaffle
         * @param snitch
         * @param bludgers
         * @param pileOfShit
         */
        Environment(Config config, std::shared_ptr<Team> team1, std::shared_ptr<Team> team2, std::shared_ptr<Quaffle> quaffle,
                std::shared_ptr<Snitch> snitch, std::array<std::shared_ptr<Bludger>, 2> bludgers, std::deque<std::shared_ptr<CubeOfShit>> pileOfShit);

        bool operator==(const Environment &other) const;
        bool operator!=(const Environment &other) const;

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
         * Gets all Positions which are not out of bounds
         * @return
         */
        static auto getAllValidCells() -> std::array<Position, 193>;

        /**
         * tests if two players are in the same team.
         * @param p1 player 1.
         * @param p2 player 2.
         * @return if the players are in the same team true, else false.
         */
        auto arePlayerInSameTeam(const std::shared_ptr<const Player>& p1, const std::shared_ptr<const Player>& p2) const -> bool;

        /**
         * checks if a player is in the own restricted zone.
         * @param player the player.
         * @return true if player is in own restricted zone, else false;
         */
        auto isPlayerInOwnRestrictedZone(const std::shared_ptr<const Player> &player) const -> bool;

        /**
         * checks if a player is in the opponent restricted zone.
         * @param player the player.
         * @return true if player is in opponent restricted zone, else false;
         */
        auto isPlayerInOpponentRestrictedZone(const std::shared_ptr<const Player> &player) const  -> bool;

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
        auto getTeamMates(const std::shared_ptr<const Player>& player) const -> std::array<std::shared_ptr<Player>, 6>;

        /**
         * Gets all players from the opponent team of the given player
         * @param player
         * @return
         */
        auto getOpponents(const std::shared_ptr<const Player>& player) const -> std::array<std::shared_ptr<Player>, 7>;

        /**
         * Determines whether the given Position is occupied by a Player or Ball
         * @param position the position to be checked
         * @return true if not occupied by any ball or player, false otherwise
         */
        bool cellIsFree(const Position &position) const;

        /**
         * get all Positions around a given position where no other player or ball is on. If all surrounding
         * cells are blocked the search window is enlarged until a free cell is found
         * @param position the position to be checked
         * @return
         */
        auto getAllFreeCellsAround(const Position &position) const -> std::vector<Position>;

        /**
         * Gets all Positions around the given Position where a player is allowed to move without risking a foul
         * @param position the Position to check
         * @param leftTeam whether to calculate Position for the left Team
         * @return a list with all found Positions, may be empty
         */
        auto getAllLegalCellsAround(const Position &position, bool leftTeam) const ->
            std::vector<Position>;

        /**
         * Gets all Positions around a given Position where no Object is located. If all surrounding
         * Positions are occupied, an empty list is returned
         * @param position the Position to be checked
         * @return
         */
        auto getAllEmptyCellsAround(const Position &position) const -> std::vector<Position>;

        /**
         * Returns player object (if not banned) at the specified position if one exists
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
        auto getTeam(const std::shared_ptr<const Player>& player) const -> std::shared_ptr<Team>;


        /**
         * Gets the team on the specified side
         * @param side the side of the team
         * @return
         */
        auto getTeam(TeamSide side) const -> std::shared_ptr<Team>;


        /**
         * Gets all valid cells not occupied by players
         * @return
         */
        auto getAllFreeCells() const -> std::vector<Position>;

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
         * Removes all the cubes of shit spawned during the last round
         */
        void removeDeprecatedShit();

        /**
         * removes Shit on a given Position
         * @param position is the Position of the CubeOfShit which should be removed
         */
        void removeShitOnCell(const Position &position);

        /**
         *
         * @param position is the Position to test
         * @return returns true, if Shit is on the Position, false otherwise
         */
        auto isShitOnCell(const Position &position) const -> bool;

        /**
         * returns a deep copy of the current Environment
         * @return
         */
        auto clone() const -> std::shared_ptr<Environment>;

        /**
         *
         * @param teamSide Teamside of the Player being redeployed
         * @param env the current Enivironment
         * @return list of possible Positions for redeploy
         */
        auto getFreeCellsForRedeploy(const gameModel::TeamSide &teamSide)const -> const std::vector<gameModel::Position>;
    };

    void to_json(nlohmann::json &j, const Position &position);
    void to_json(nlohmann::json &j, const Object &object);
    void to_json(nlohmann::json &j, const Player &player);
    void to_json(nlohmann::json &j, const Snitch &snitch);
    void to_json(nlohmann::json &j, const Fanblock &fanblock);
    void to_json(nlohmann::json &j, const Team &team);
    void to_json(nlohmann::json &j, const Config &config);
    void to_json(nlohmann::json &j, const Environment &environment);

    void from_json(const nlohmann::json &j, Position &position);
    void from_json(const nlohmann::json &j, Object &object);
    void from_json(const nlohmann::json &j, Player &player);
    void from_json(const nlohmann::json &j, Snitch &snitch);
    void from_json(const nlohmann::json &j, Fanblock &fanblock);
    void from_json(const nlohmann::json &j, Team &team);
    void from_json(const nlohmann::json &j, Config &config);
    void from_json(const nlohmann::json &j, Environment &environment);

}


#endif //GAMELOGIC_SOPRAGAMEMODEL_H