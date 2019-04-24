#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include <utility>

#include "sopraGameModel.h"

#include <iostream>

Player::Player(int posX, int posY, std::string  name, Gender gender, Broom broom) :
x(posX), y(posY), name(std::move(name)), gender(gender), broom(broom){

}

Ball::Ball(int posX, int posY) {
    this->x = posX;
    this->y = posY;
}

Team::Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
           std::string name, std::string colorMain, std::string colorSecondary, const Fanblock &fanblock) :
           name(std::move(name)), colorMain(std::move(colorMain)), colorSecondary(std::move(colorSecondary)), fanblock(fanblock){
    this->seeker = std::move(seeker);
    this->keeper = std::move(keeper);
    this->beaters = std::move(beaters);
    this->chasers = std::move(chasers);
}

Fanblock::Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush){
    if(teleportation + rangedAttack + impulse + snitchPush != 7){
        throw std::invalid_argument("Fanblock has to contain exactly 7 fans!");
    }

    this->teleportation = teleportation;
    this->impulse = impulse;
    this->rangedAttack = rangedAttack;
    this->snitchPush = snitchPush;
}

Cell Environment::getCell(unsigned int x, unsigned int y) {
    if(x >= 17 || y >= 13) {
        return Cell::OutOfBounds;
    }else if((x == 2 || x == 14) && (y == 4 || y == 6 || y == 8)){
        return Cell::Goal;
    } else if(x > 6 && x < 10 && y > 4 && y < 8){
        return Cell::Centre;
    } else if(x > 4 && x < 12){
        return Cell::Standard;
    } else if((x == 4 || x == 12) && (y < 4 || y > 8)){
        return Cell::Standard;
    } else if((x == 3 || x == 13) && (y < 2 || y > 10)){
        return Cell::Standard;
    } else if(x > 1 && x < 15 && y > 0 && y < 12){
        return Cell::Restricted;
    } else if(x > 0 && x < 16 && y > 1 && y < 11){
        return Cell::Restricted;
    } else if(x >= 0 && y > 3 && y < 9){
        return Cell::Restricted;
    } else{
        return Cell::OutOfBounds;
    }
}

Environment::Environment(Team team1, Team team2, Quaffle quaffle, Snitch snitch,
                         std::array<Bludger, 2> bludgers)
        : team1(std::move(team1)), team2(std::move(team2)), quaffle(quaffle), snitch(snitch), bludgers(bludgers) {}

Snitch::Snitch(int x, int y): Ball(x, y) {}

Bludger::Bludger(int x, int y) : Ball(x, y) {}

Quaffle::Quaffle(int x, int y) : Ball(x, y) {}

Chaser::Chaser(int posX, int posY, std::string name, Gender gender, Broom broom) :
Player(posX, posY, std::move(name), gender, broom) {}

Keeper::Keeper(int posX, int posY, std::string name, Gender gender, Broom broom) :
Player(posX, posY, std::move(name), gender, broom) {}

Seeker::Seeker(int posX, int posY, std::string name, Gender gender, Broom broom) :
Player(posX, posY, std::move(name), gender, broom) {}

Beater::Beater(int posX, int posY, std::string name, Gender gender, Broom broom) :
Player(posX, posY, std::move(name), gender, broom) {}
