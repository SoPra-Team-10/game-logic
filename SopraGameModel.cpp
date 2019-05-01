#include "SopraGameModel.h"
#include <utility>
#include <iostream>
#include <cmath>

namespace gameModel{

    Player::Player(Position position, std::string  name, Gender gender, Broom broom) :
            position{position}, name(std::move(name)), gender(gender), broom(broom){}

    bool Player::operator==(const Player &other) const {
        return position == other.position && name == other.name &&
        gender == other.gender && broom == other.broom;
    }

    bool Player::operator!=(const Player &other) const {
        return !(*this == other);
    }

    Ball::Ball(Position position) : position{position} {}


    Fanblock::Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush) : fans(){
        if(teleportation + rangedAttack + impulse + snitchPush != 7){
            throw std::invalid_argument("Fanblock has to contain exactly 7 fans!");
        }

        using fan = InterferenceType ;
        fans.emplace(fan::RangedAttack, rangedAttack);
        fans.emplace(fan::Teleport, teleportation);
        fans.emplace(fan::Impulse, impulse);
        fans.emplace(fan::SnitchPush, snitchPush);
    }

    int Fanblock::getUses(InterferenceType fan) {
        return fans.at(fan);
    }

    void Fanblock::banFan(InterferenceType fan) {
        if(fans.at(fan) <= 0){
            throw std::invalid_argument("No uses left");
        }

        fans.at(fan)--;
    }

    Cell Environment::getCell(int x, int y) {
        if(x >= 17 || y >= 13) {
            return Cell::OutOfBounds;
        }else if((x == 2 || x == 14) && (y == 4 || y == 6 || y == 8)){
            return x < 8 ? Cell::GoalLeft : Cell::GoalRight;
        } else if(x > 6 && x < 10 && y > 4 && y < 8){
            return Cell::Centre;
        } else if(x > 4 && x < 12){
            return Cell::Standard;
        } else if((x == 4 || x == 12) && (y < 4 || y > 8)){
            return Cell::Standard;
        } else if((x == 3 || x == 13) && (y < 2 || y > 10)){
            return Cell::Standard;
        } else if(x > 1 && x < 15 && y > 0 && y < 12){
            return x < 8 ? Cell::RestrictedLeft : Cell::RestrictedRight;
        } else if(x > 0 && x < 16 && y > 1 && y < 11){
            return x < 8 ? Cell::RestrictedLeft : Cell::RestrictedRight;
        } else if(y > 3 && y < 9){
            return x < 8 ? Cell::RestrictedLeft : Cell::RestrictedRight;
        } else{
            return Cell::OutOfBounds;
        }
    }

    Cell Environment::getCell(Position position) {
        return getCell(position.x, position.y);
    }

    Environment::Environment(const Config config,Team team1, Team team2, Quaffle quaffle, Snitch snitch,
            std::array<Bludger, 2> bludgers)
            : config(config), team1(std::move(team1)), team2(std::move(team2)), quaffle(quaffle), snitch(snitch),
            bludgers(bludgers) {}

    Player Environment::getPlayerOnCell(const Position &position) {

        return Player();
    }

    bool Environment::arePlayerInSameTeam(const Player &p1, const Player &p2) const {
        return (this->team1.hasMember(p1) && this->team1.hasMember(p2)) ||
               (this->team2.hasMember(p1) && this->team2.hasMember(p2));
    }

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

    auto Team::getAllPlayers() const -> std::vector<std::shared_ptr<const Player>> {
        std::vector<std::shared_ptr<const Player>> ret;
        ret.reserve(7);
        for(const auto& p : beaters){
            ret.push_back(std::make_shared<const Player>(p));
        }

        for(const auto& p : chasers){
            ret.push_back(std::make_shared<const Player>(p));
        }

        ret.push_back(std::make_shared<const Player>(keeper));
        ret.push_back(std::make_shared<const Player>(seeker));
        return ret;
    }

    bool Team::hasMember(const Player &player) const {
        for(const auto &p : getAllPlayers()){
            if(player == *p){
                return true;
            }
        }

        return false;
    }

    Config::Config(unsigned int maxRounds, const Timeouts &timeouts, const FoulDetectionProbs &foulDetectionProbs,
                   const GameDynamicsProbs &gameDynamicsProbs, std::map<Broom, double> extraTurnProbs) :
            maxRounds(maxRounds), timeouts(timeouts), foulDetectionProbs(foulDetectionProbs), gameDynamicsProbs(gameDynamicsProbs),
            extraTurnProbs(std::move(extraTurnProbs)) {}

    double Config::getExtraTurnProb(Broom broom) const{
        return extraTurnProbs.at(broom);
    }

    Position::Position(int x, int y) {
        this->x = x;
        this->y = y;
    }

    bool Position::operator==(const Position &other) const{
        return this->x == other.x && this->y == other.y;
    }

    bool Position::operator!=(const Position &other) const{
        return !(*this == other);
    }

    Vector::Vector(double x, double y) {
        this->x = x;
        this->y = y;
    }

    double Vector::abs() const{
        return std::sqrt(pow(this->x, 2) + pow(this->x, 2));
    }

    void Vector::normalize(){
        double a = this->abs();
        this->x = this->x * (1 / a);
        this->y = this->y * (1 / a);
    }

    bool Vector::operator==(const Vector &v) const{
        return this->x == v.x && this->y == v.y;
    }

    Vector Vector::operator*(const double &c) const{
        return Vector(this->x * c, this->y * c);
    }

    Vector Vector::operator+(const Vector &v) const{
        return Vector(this->x + v.x, this->y + v.y);
    }

    Position Vector::operator+(const Position &p) const{
        return Position(static_cast<int>(p.x + round(this->x)),
                static_cast<int>(p.y + round(this->y)));
    }
}
