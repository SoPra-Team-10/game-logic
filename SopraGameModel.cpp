#include "SopraGameModel.h"
#include <utility>
#include <iostream>

Player::Player(Position position, std::string  name, Gender gender, Broom broom) :
position{position}, name(std::move(name)), gender(gender), broom(broom){}

Ball::Ball(Position position) : position{position} {}


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

Snitch::Snitch(Position position): Ball(position) {}

Bludger::Bludger(Position position) : Ball(position) {}

Quaffle::Quaffle(Position position) : Ball(position) {}

Chaser::Chaser(Position position, std::string name, Gender gender, Broom broom) :
Player(position, std::move(name), gender, broom) {}

Keeper::Keeper(Position position, std::string name, Gender gender, Broom broom) :
Player(position, std::move(name), gender, broom) {}

Seeker::Seeker(Position position, std::string name, Gender gender, Broom broom) :
Player(position, std::move(name), gender, broom) {}

Beater::Beater(Position position, std::string name, Gender gender, Broom broom) :
Player(position, std::move(name), gender, broom) {}

Team::Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
           std::string  name, std::string  colorMain, std::string  colorSecondary,
           const Fanblock &fanblock)
        : seeker(std::move(seeker)), keeper(std::move(keeper)), beaters(std::move(beaters)), chasers(std::move(chasers)), name(std::move(name)), colorMain(std::move(colorMain)),
          colorSecondary(std::move(colorSecondary)), fanblock(fanblock) {}

