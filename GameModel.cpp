#include <utility>
#include "GameModel.h"
#include "GameController.h"
#include <utility>
#include <iostream>
#include <cmath>

namespace gameModel{

    Player::Player(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
        Object(position, id), name(std::move(name)), gender(gender), broom(broom), isFined{false} {
    }

    bool Player::operator==(const Player &other) const {
        return position == other.position && name == other.name &&
        gender == other.gender && broom == other.broom && id == other.id;
    }

    bool Player::operator!=(const Player &other) const {
        return !(*this == other);
    }


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

    Cell Environment::getCell(const Position &position) {
        return getCell(position.x, position.y);
    }

    auto Environment::getSurroundingPositions(const Position &position) -> std::vector<Position>{
        std::vector<Position> ret;
        ret.reserve(8);
        for(int x = position.x - 1; x <= position.x + 1; x++){
            for(int y = position.y - 1; y <= position.y + 1; y++){
                Position curr(x, y);
                if(curr != position && getCell(curr) != Cell::OutOfBounds){
                    ret.emplace_back(curr);
                }
            }
        }

        return ret;
    }

    Environment::Environment(Config config, std::shared_ptr<Team> team1, std::shared_ptr<Team> team2) : config(std::move(config)), team1(std::move(team1)),
            team2(std::move(team2)), quaffle(std::make_shared<Quaffle>()), snitch(std::make_shared<Snitch>()),
            bludgers{std::make_shared<Bludger>(communication::messages::types::EntityId::BLUDGER1),
            std::make_shared<Bludger>(communication::messages::types::EntityId::BLUDGER2)} {}

    Environment::Environment(Config config, std::shared_ptr<Team> team1, std::shared_ptr<Team> team2, std::shared_ptr<Quaffle> quaffle, std::shared_ptr<Snitch> snitch,
                             std::array<std::shared_ptr<Bludger>, 2> bludgers) : config(std::move(config)), team1(std::move(team1)),
                             team2(std::move(team2)), quaffle(std::move(quaffle)), snitch(std::move(snitch)),
                             bludgers(std::move(bludgers)){}


    Environment::Environment(communication::messages::broadcast::MatchConfig matchConfig, const communication::messages::request::TeamConfig& teamConfig1,
                const communication::messages::request::TeamConfig& teamConfig2, communication::messages::request::TeamFormation teamFormation1,
                communication::messages::request::TeamFormation teamFormation2) :
                             Environment({matchConfig}, std::make_shared<Team>(teamConfig1, teamFormation1, true),
                                     std::make_shared<Team>(teamConfig2, teamFormation2, false)){}

    auto Environment::getAllPlayers() const -> std::array<std::shared_ptr<Player>, 14> {
        std::array<std::shared_ptr<Player>, 14> ret;
        auto it = ret.begin();
        for(const auto &p : team1->getAllPlayers()){
            *it = p;
            it++;
        }

        for(const auto &p : team2->getAllPlayers()){
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
        resultVect.reserve(8);

        int startX = position.x - 1;
        int endX = position.x + 1;
        int startY = position.y - 1;
        int endY = position.y + 1;

        do {
            for (int yPos = startY; yPos <= endY; yPos++) {
                for (int xPos = startX; xPos <= endX; xPos++) {
                    if (xPos == position.x && yPos == position.y) {
                        continue;
                    }
                    else if (Environment::getCell(xPos, yPos) != Cell::OutOfBounds && !this->getPlayer({xPos, yPos}).has_value()) {
                        resultVect.emplace_back(Position(xPos, yPos));
                    }
                }
            }

            startX--;
            endX++;
            startY--;
            endY++;

        } while (resultVect.empty());

        return resultVect;
    }

    auto Environment::getTeamMates(const std::shared_ptr<Player>& player) const -> std::array<std::shared_ptr<Player>, 6> {
        auto players = team1->hasMember(player) ? team1->getAllPlayers() : team2->getAllPlayers();
        std::array<std::shared_ptr<Player>, 6> ret;
        auto it = ret.begin();
        for(const auto &p : players){
            if(*p != *player){
                *it = p;
                it++;
            }
        }

        return ret;
    }

    auto Environment::getOpponents(const std::shared_ptr<Player>& player) const -> std::array<std::shared_ptr<Player>, 7> {
        return team1->hasMember(player) ? team2->getAllPlayers() : team1->getAllPlayers();
    }

    auto Environment::getPlayer(const Position &position) const -> std::optional<std::shared_ptr<Player>> {
        for(const auto &p : getAllPlayers()){
            if(p->position == position){
                return p;
            }
        }

        return {};
    }

    auto Environment::arePlayerInSameTeam(const std::shared_ptr<Player>& p1, const std::shared_ptr<Player>& p2) const -> bool {
        return (this->team1->hasMember(p1) && this->team1->hasMember(p2)) ||
               (this->team2->hasMember(p1) && this->team2->hasMember(p2));
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

    auto Environment::getAllFreeCells() -> std::array<Position, 179> {
        //@TODO optimization!!!
        std::array<Position, 179> ret{};
        auto it = ret.begin();
        for(const auto &cell : getAllValidCells()){
            if(cellIsFree(cell)){
                *it = cell;
                if (it < ret.end() ) {
                    it++;
                }
                else {
                    throw std::runtime_error("There are less than 14 players on the field!");
                }
            }
        }

        return ret;
    }

    auto Environment::isPlayerInOwnRestrictedZone(const std::shared_ptr<Player>& player) const -> bool {
        if (this->team1->hasMember(player) && this->getCell(player->position) == Cell::RestrictedLeft) {
            return true;
        }
        if (this->team2->hasMember(player) && this->getCell(player->position) == Cell::RestrictedRight) {
            return true;
        }

        return false;
    }
    auto Environment::isPlayerInOpponentRestrictedZone(const std::shared_ptr<Player>& player) const  -> bool {
        if (this->team1->hasMember(player) && this->getCell(player->position) == Cell::RestrictedRight) {
            return true;
        }
        if (this->team2->hasMember(player) && this->getCell(player->position) == Cell::RestrictedLeft) {
            return true;
        }
        return false;
    }

    auto Environment::getTeam(const std::shared_ptr<Player>& player) const -> std::shared_ptr<Team> {
        if (this->team1->hasMember(player)) {
            return this->team1;
        }
        else if (this->team2->hasMember(player)) {
            return this->team2;
        }

        throw std::runtime_error("The Player isn't part of any team.");
    }

    void Environment::placePlayerOnRandomFreeCell(const std::shared_ptr<Player>& player) {
        std::vector<Position> possibleCells;
        possibleCells.reserve(82);
        for(const auto &cell : getAllValidCells()){
            if(getCell(cell) == Cell::Centre || getCell(cell) == Cell::GoalLeft){
                continue;
            }

            if(team1->hasMember(player) && cell.x < 8 && cellIsFree(cell)){
                possibleCells.push_back(cell);
            }

            if(team2->hasMember(player) && cell.x > 8 && cellIsFree(cell)){
                possibleCells.push_back(cell);
            }
        }

        int index = gameController::rng(0, static_cast<int>(possibleCells.size() - 1));
        player->position = possibleCells[index];
    }

     auto Environment::getGoalsLeft() -> std::array<Position, 3> {
        return {Position{2, 4}, Position{2, 6}, Position{2, 8}};
    }

    auto Environment::getGoalsRight() -> std::array<Position, 3> {
        return {Position{14, 4}, Position{14, 6}, Position{14, 8}};
    }



    // Ball Types

    Ball::Ball(Position position, communication::messages::types::EntityId id) : Object(position, id) {}

    Snitch::Snitch(Position position): Ball(position, communication::messages::types::EntityId::SNITCH) {}

    Snitch::Snitch() : Ball({0, 0}, communication::messages::types::EntityId::SNITCH), exists(false){
        auto allCells = Environment::getAllValidCells();
        auto index = gameController::rng(0, static_cast<int>(allCells.size()) - 1);
        position = allCells[index];
    }

    Bludger::Bludger(Position position, communication::messages::types::EntityId id) : Ball(position, id) {}

    Bludger::Bludger(communication::messages::types::EntityId id) : Ball({8, 6}, id){}

    Quaffle::Quaffle(Position position) : Ball(position, communication::messages::types::EntityId::QUAFFLE) {}

    Quaffle::Quaffle() : Ball({8, 6}, communication::messages::types::EntityId::QUAFFLE){}

    Chaser::Chaser(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, std::move(name), gender, broom, id) {}

    Keeper::Keeper(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, std::move(name), gender, broom, id) {}

    Seeker::Seeker(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, std::move(name), gender, broom, id) {}

    Beater::Beater(Position position, std::string name, communication::messages::types::Sex gender, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, std::move(name), gender, broom, id) {}


    // Team

    Team::Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers,
               std::string  name, std::string  colorMain, std::string  colorSecondary,
               Fanblock fanblock) :
               seeker(std::make_shared<Seeker>(seeker)), keeper(std::make_shared<Keeper>(keeper)), beaters{std::make_shared<Beater>(beaters[0]), std::make_shared<Beater>(beaters[1])},
                chasers{std::make_shared<Chaser>(chasers[0]), std::make_shared<Chaser>(chasers[1]), std::make_shared<Chaser>(chasers[2])},name(std::move(name)),
                colorMain(std::move(colorMain)), colorSecondary(std::move(colorSecondary)), fanblock(std::move(fanblock)) {}

   Team::Team(const communication::messages::request::TeamConfig& tConf, communication::messages::request::TeamFormation tForm, bool leftTeam) :
   seeker(std::make_shared<Seeker>(Position{tForm.getSeekerX(), tForm.getSeekerY()}, tConf.getSeeker().getName(), tConf.getSeeker().getSex(), tConf.getSeeker().getBroom(), leftTeam ?
   communication::messages::types::EntityId::LEFT_SEEKER : communication::messages::types::EntityId::RIGHT_SEEKER)),
   keeper(std::make_shared<Keeper>(Position{tForm.getKeeperX(), tForm.getKeeperY()}, tConf.getKeeper().getName(), tConf.getKeeper().getSex(), tConf.getKeeper().getBroom(), leftTeam ?
   communication::messages::types::EntityId::LEFT_KEEPER : communication::messages::types::EntityId::RIGHT_KEEPER)),
   beaters{std::make_shared<Beater>(Position{tForm.getBeater1X(), tForm.getBeater1Y()}, tConf.getBeater1().getName(), tConf.getBeater1().getSex(), tConf.getBeater1().getBroom(), leftTeam ?
            communication::messages::types::EntityId::LEFT_BEATER1 : communication::messages::types::EntityId::RIGHT_BEATER1),
            std::make_shared<Beater>(Position{tForm.getBeater2X(), tForm.getBeater2Y()}, tConf.getBeater2().getName(), tConf.getBeater2().getSex(), tConf.getBeater2().getBroom(), leftTeam ?
            communication::messages::types::EntityId ::LEFT_BEATER2 : communication::messages::types::EntityId::RIGHT_BEATER2)},
   chasers{std::make_shared<Chaser>(Position{tForm.getChaser1X(), tForm.getChaser1Y()}, tConf.getChaser1().getName(), tConf.getChaser1().getSex(), tConf.getChaser1().getBroom(), leftTeam ?
            communication::messages::types::EntityId::LEFT_CHASER1 : communication::messages::types::EntityId::RIGHT_CHASER1),
            std::make_shared<Chaser>(Position{tForm.getChaser2X(), tForm.getChaser2Y()}, tConf.getChaser2().getName(), tConf.getChaser2().getSex(), tConf.getChaser2().getBroom(), leftTeam ?
            communication::messages::types::EntityId::LEFT_CHASER2 : communication::messages::types::EntityId::RIGHT_CHASER2),
            std::make_shared<Chaser>(Position{tForm.getChaser3X(), tForm.getChaser3Y()}, tConf.getChaser3().getName(), tConf.getChaser3().getSex(), tConf.getChaser3().getBroom(), leftTeam ?
            communication::messages::types::EntityId::LEFT_CHASER3 : communication::messages::types::EntityId::RIGHT_CHASER3)},
   name(tConf.getTeamName()), colorMain(tConf.getColorPrimary()), colorSecondary(tConf.getColorSecondary()),
   fanblock(tConf.getElfs(), tConf.getGoblins(), tConf.getTrolls(), tConf.getNifflers()){}


    auto Team::getAllPlayers() const -> std::array<std::shared_ptr<Player>, 7> {
        std::array<std::shared_ptr<Player>, 7> ret;
        auto it = ret.begin();
        for(const auto& p : beaters){
            *it = p;
            it++;
        }

        for(const auto& p : chasers){
            *it = p;
            it++;
        }

        ret[5] = keeper;
        ret[6] = seeker;
        return ret;
    }

    bool Team::hasMember(const std::shared_ptr<Player>& player) const {
        for(const auto &p : getAllPlayers()){
            if(player == p){
                return true;
            }
        }

        return false;
    }

    int Team::numberOfBannedMembers() {
        int ret = 0;
        for(const auto &player :getAllPlayers()){
            if(player->isFined){
                ret++;
            }
        }

        return ret;
    }

    // Config

    Config::Config(unsigned int maxRounds, const Timeouts &timeouts, const FoulDetectionProbs &foulDetectionProbs,
                   const GameDynamicsProbs &gameDynamicsProbs, std::map<communication::messages::types::Broom, double> extraTurnProbs) :
            maxRounds(maxRounds), timeouts(timeouts), foulDetectionProbs(foulDetectionProbs), gameDynamicsProbs(gameDynamicsProbs),
            extraTurnProbs(std::move(extraTurnProbs)) {}

    double Config::getExtraTurnProb(communication::messages::types::Broom broom) const{
        return extraTurnProbs.at(broom);
    }

    //Willste mal nen richtig großen ... KONSTRUKTOR sehen? ;)
    Config::Config(const communication::messages::broadcast::MatchConfig &config) : maxRounds(config.getMaxRounds()),
        timeouts{config.getPlayerTurnTimeout(), config.getFanTurnTimeout(), config.getPlayerPhaseTime(), config.getFanPhaseTime(),
                 config.getBallPhaseTime()}, foulDetectionProbs{config.getProbFoulFlacking(), config.getProbFoulHaversacking(),
                 config.getProbFoulStooging(), config.getProbFoulBlatching(), config.getProbFoulSnitchnip(), config.getProbFoulElf(),
                 config.getProbFoulGoblin(), config.getProbFoulTroll(), config.getProbFoulSnitch()},
                 gameDynamicsProbs{config.getProbThrowSuccess(), config.getProbKnockOut(), config.getProbFoolAway(), config.getProbCatchSnitch(),
                 config.getProbCatchQuaffle(), config.getProbWrestQuaffle()}{}


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

    Object::Object(const Position &position, communication::messages::types::EntityId id) : position(position), id(id){}
}
