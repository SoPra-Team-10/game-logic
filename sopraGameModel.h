#ifndef GAMELOGIC_SOPRAGAMEMODEL_H
#define GAMELOGIC_SOPRAGAMEMODEL_H

#include <string>
#include <array>

/**
 * Types of the playing field's cells
 */
enum class Cell{
    Goal, ///< Goal cell
    Standard, ///< Cell with no specialties
    Centre, ///< Cells belonging to the fields centre area
    Restricted, ///< Restricted area where only one attacker at a time is allowed
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

/**
 * Represents the playable characters
 */
class Player{
public:
    int x{}, y{};
    std::string name;
    Gender gender = Gender::Female;
    Broom broom = Broom::Cleansweep_11;

    Player() = default;
    Player(int posX, int posY, std::string  name, Gender gender, Broom broom);
};

/**
 * Represents non playable ball-objects
 */
class Ball{
public:
    int x, y;

    Ball(int posX, int posY);
};

/**
 * Represents available fans for a Team
 */
class Fanblock{
public:
    Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush);
    int teleportation, rangedAttack, impulse, snitchPush;

};

class Chaser : public Player{
public:
    Chaser(int posX, int posY, std::string  name, Gender gender, Broom broom);
};

class Keeper : public Player{
public:
    Keeper(int posX, int posY, std::string  name, Gender gender, Broom broom);
};

class Seeker : public Player{
public:
    Seeker(int posX, int posY, std::string  name, Gender gender, Broom broom);
};

class Beater : public Player{
public:
    Beater(int posX, int posY, std::string  name, Gender gender, Broom broom);
};

class Quaffle : public Ball{
public:
    Quaffle(int x, int y);
};

class Bludger : public Ball{
public:
    Bludger(int x, int y);
};

class Snitch : public Ball{
public:
    bool exists = false;

    Snitch(int x, int y);
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
    Cell getCell(unsigned int x, unsigned int y);
};

#endif //GAMELOGIC_SOPRAGAMEMODEL_H