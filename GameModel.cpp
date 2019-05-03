#include <utility>
#include "GameModel.h"
#include "GameController.h"
#include <utility>
#include <iostream>
#include <cmath>

namespace gameModel{

    // Player

    Player::Player(Position position, std::string  name, Gender gender, Broom broom) :
            position{position}, name(std::move(name)), gender(gender), broom(broom) {}

    bool Player::operator==(const Player &other) const {
        return position == other.position && name == other.name &&
        gender == other.gender && broom == other.broom;
    }

    bool Player::operator!=(const Player &other) const {
        return !(*this == other);
    }


    // Ball

    Ball::Ball(Position position) : position{position} {}


    // Fanblock

    Fanblock::Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush) : fans() {
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


    // Environment

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

    Environment::Environment(Config config,Team team1, Team team2) : config(std::move(config)), team1(std::move(team1)),
    team2(std::move(team2)), quaffle(), snitch(), bludgers() {}

    Environment::Environment(Config config, Team team1, Team team2, Quaffle quaffle, Snitch snitch,
                             std::array<Bludger, 2> bludgers) : config(std::move(config)), team1(std::move(team1)),
                             team2(std::move(team2)), quaffle(std::move(quaffle)), snitch(std::move(snitch)),
                             bludgers(std::move(bludgers)){}

    auto Environment::getAllPlayers() const -> std::array<std::shared_ptr<Player>, 14> {
        std::array<std::shared_ptr<Player>, 14> ret;
        auto it = ret.begin();
        for(const auto &p : team1.getAllPlayers()){
            *it = p;
            it++;
        }

        for(const auto &p : team2.getAllPlayers()){
            *it = p;
            it++;
        }

        return ret;
    }

    bool Environment::cellIsFree(const Position &position) const {
        for(const auto &p : getAllPlayers()){
            if(position == p->position){
                return false;
            }
        }

        return true;
    }

    auto Environment::getAllPlayerFreeCellsAround(const Position &position) const -> std::vector<Position> {
        std::vector<Position> resultVect;

        int startX = position.x - 1;
        int endX = position.x + 1;
        int startY = position.y - 1;
        int endY = position.y + 1;

        do {
            for (int i = startY; i <= endY; i++) {
                for (int j = startX; j <= endX; j++) {
                    if (i == position.x && j == position.y) {
                        continue;
                    }
                    if (Environment::getCell(j, i) != Cell::OutOfBounds && this->getPlayer(Position(j, i)) == nullptr) {
                        resultVect.emplace_back(Position(j, i));
                    }
                }
            }

            startX--;
            endX++;
            startX--;
            endX++;

        } while (resultVect.empty());

        return resultVect;
    }

    auto Environment::getTeamMates(const Player &player) const -> std::array<std::shared_ptr<Player>, 6> {
        auto players = team1.hasMember(player) ? team1.getAllPlayers() : team2.getAllPlayers();
        std::array<std::shared_ptr<Player>, 6> ret;
        auto it = ret.begin();
        for(const auto &p : players){
            if(*p != player){
                *it = p;
                it++;
            }
        }

        return ret;
    }

    auto Environment::getOpponents(const Player &player) const -> std::array<std::shared_ptr<Player>, 7> {
        return team1.hasMember(player) ? team2.getAllPlayers() : team1.getAllPlayers();
    }

    auto Environment::getPlayer(Position position) const -> std::optional<std::shared_ptr<Player>> {
        for(const auto &p : getAllPlayers()){
            if(p->position == position){
                return p;
            }
        }

        return {};
    }

    auto Environment::arePlayerInSameTeam(const Player &p1, const Player &p2) const -> bool {
        return (this->team1.hasMember(p1) && this->team1.hasMember(p2)) ||
               (this->team2.hasMember(p1) && this->team2.hasMember(p2));
    }

    auto Environment::getAllValidCells() -> std::array<Position, 193> {
        std::array<Position, 193> ret{};
        auto it = ret.begin();
        for(int x = 0; x < 17; x++){
            for(int y = 0; y < 13; y++){
                if(getCell(x, y) != Cell::OutOfBounds){
                    *it = {x, y};
                    it++;
                }
            }
        }

        return ret;
    }

    auto Environment::isPlayerInOwnRestrictedZone(const Player &player) const -> bool {
        if (this->team1.hasMember(player) && this->getCell(player.position) == Cell::RestrictedLeft) {
            return true;
        }
        if (this->team2.hasMember(player) && this->getCell(player.position) == Cell::RestrictedRight) {
            return true;
        }

        return false;
    }

    auto Environment::isPlayerInOpponentRestrictedZone(const Player &player) const  -> bool {
        if (this->team1.hasMember(player) && this->getCell(player.position) == Cell::RestrictedRight) {
            return true;
        }
        if (this->team2.hasMember(player) && this->getCell(player.position) == Cell::RestrictedLeft) {
            return true;
        }
        return false;
    }

    // Ball Types

    Snitch::Snitch(Position position): Ball(position) {}

    Snitch::Snitch() : Ball({0, 0}), exists(false) {
        auto allCells = Environment::getAllValidCells();
        auto index = gameController::rng(0, static_cast<int>(allCells.size()));
        position = allCells[index];
    }

    Bludger::Bludger(Position position) : Ball(position) {}

    Bludger::Bludger() : Ball({8, 6}) {}

    Quaffle::Quaffle(Position position) : Ball(position) {}

    Quaffle::Quaffle() : Ball({8, 6}) {}


    // Player Types

    Chaser::Chaser(Position position, std::string name, Gender gender, Broom broom) :
            Player(position, std::move(name), gender, broom) {}

    Keeper::Keeper(Position position, std::string name, Gender gender, Broom broom) :
            Player(position, std::move(name), gender, broom) {}

    Seeker::Seeker(Position position, std::string name, Gender gender, Broom broom) :
            Player(position, std::move(name), gender, broom) {}

    Beater::Beater(Position position, std::string name, Gender gender, Broom broom) :
            Player(position, std::move(name), gender, broom) {}


    // Team

    Team::Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
               std::string  name, std::string  colorMain, std::string  colorSecondary,
               Fanblock fanblock)
            : seeker(std::move(seeker)), keeper(std::move(keeper)), beaters(std::move(beaters)), chasers(std::move(chasers)), name(std::move(name)), colorMain(std::move(colorMain)),
              colorSecondary(std::move(colorSecondary)), fanblock(std::move(fanblock)) {}

    auto Team::getAllPlayers() const -> std::array<std::shared_ptr<Player>, 7> {
        std::array<std::shared_ptr<Player>, 7> ret;
        auto it = ret.begin();
        for(const auto& p : beaters){
            *it = std::make_shared<Player>(p);
            it++;
        }

        for(const auto& p : chasers){
            *it = std::make_shared<Player>(p);
            it++;
        }

        ret[5] = std::make_shared<Player>(keeper);
        ret[6] = std::make_shared<Player>(seeker);
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


    // Config

    Config::Config(unsigned int maxRounds, const Timeouts &timeouts, const FoulDetectionProbs &foulDetectionProbs,
                   const GameDynamicsProbs &gameDynamicsProbs, std::map<Broom, double> extraTurnProbs) :
            maxRounds(maxRounds), timeouts(timeouts), foulDetectionProbs(foulDetectionProbs), gameDynamicsProbs(gameDynamicsProbs),
            extraTurnProbs(std::move(extraTurnProbs)) {}

    double Config::getExtraTurnProb(Broom broom) const{
        return extraTurnProbs.at(broom);
    }


    // Position

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

    Position Vector::operator+(const Position &p) const{
        return Position(static_cast<int>(p.x + round(this->x)),
                        static_cast<int>(p.y + round(this->y)));
    }

    // Vector

    Vector::Vector(double x, double y) {
        this->x = x;
        this->y = y;
    }

    double Vector::abs() const{
        return std::sqrt(pow(this->x, 2) + pow(this->x, 2));
    }

    void Vector::normalize(){
        double a = this->abs();
        if(a < std::numeric_limits<double>::epsilon()){
            throw std::runtime_error("Cannot normalize Vector with length 0");
        }

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
}