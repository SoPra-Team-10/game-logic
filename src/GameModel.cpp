#include "GameModel.h"
#include "GameController.h"
#include "conversions.h"

#include <utility>
#include <iostream>
#include <cmath>
#include <utility>

namespace gameModel{

    Player::Player(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
        Object(position, id), broom(broom), isFined{false} {
    }

    bool Player::operator==(const Player &other) const {
        return position == other.position && broom == other.broom && id == other.id;
    }

    bool Player::operator!=(const Player &other) const {
        return !(*this == other);
    }


    // Fanblock

    Fanblock::Fanblock(int teleportation, int rangedAttack, int impulse, int snitchPush, int blockCell) : currFans(), initialFans() {
        using fan = InterferenceType ;
        initialFans.emplace(fan::RangedAttack, rangedAttack);
        initialFans.emplace(fan::Teleport, teleportation);
        initialFans.emplace(fan::Impulse, impulse);
        initialFans.emplace(fan::SnitchPush, snitchPush);
        initialFans.emplace(fan::BlockCell, blockCell);
        currFans.emplace(fan::RangedAttack, rangedAttack);
        currFans.emplace(fan::Teleport, teleportation);
        currFans.emplace(fan::Impulse, impulse);
        currFans.emplace(fan::SnitchPush, snitchPush);
        currFans.emplace(fan::BlockCell, blockCell);
    }

    int Fanblock::getUses(InterferenceType fan) const {
        return currFans.at(fan);
    }

    int Fanblock::getUses(communication::messages::types::FanType fan) const {
        return getUses(conversions::fanToInterference(fan));
    }

    int Fanblock::getBannedCount(gameModel::InterferenceType fan) const {
        return initialFans.at(fan) - currFans.at(fan);
    }

    int Fanblock::getBannedCount(communication::messages::types::FanType fan) const {
        return getBannedCount(conversions::fanToInterference(fan));
    }

    void Fanblock::banFan(InterferenceType fan) {
        if(--currFans.at(fan) < 0){
            throw std::runtime_error("No fans left to ban!");
        }
    }

    void Fanblock::banFan(communication::messages::types::FanType fan) {
        banFan(conversions::fanToInterference(fan));
    }


    // Environment

    Cell Environment::getCell(int x, int y) {
        if(x >= 17 || y >= 13 || x < 0 || y < 0) {
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
                             std::array<std::shared_ptr<Bludger>, 2> bludgers, std::deque<std::shared_ptr<CubeOfShit>> pileOfShit) : config(std::move(config)),
                             team1(std::move(team1)), team2(std::move(team2)), quaffle(std::move(quaffle)), snitch(std::move(snitch)),
                             bludgers(std::move(bludgers)), pileOfShit(std::move(pileOfShit)){}


    Environment::Environment(communication::messages::broadcast::MatchConfig matchConfig, const communication::messages::request::TeamConfig& teamConfig1,
                const communication::messages::request::TeamConfig& teamConfig2, communication::messages::request::TeamFormation teamFormation1,
                communication::messages::request::TeamFormation teamFormation2) :
                             Environment({matchConfig}, std::make_shared<Team>(teamConfig1, teamFormation1, TeamSide::LEFT),
                                     std::make_shared<Team>(teamConfig2, teamFormation2, TeamSide::RIGHT)){}

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
        return !getPlayer(position).has_value() && !(snitch->exists && snitch->position == position) &&
                quaffle->position != position && bludgers[0]->position != position && bludgers[1]->position != position;
    }

    auto Environment::getAllFreeCellsAround(const Position &position) const -> std::vector<Position> {
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
                    else if (Environment::getCell(xPos, yPos) != Cell::OutOfBounds) {
                        if (!cellIsFree({xPos, yPos})) {
                            continue;
                        }
                        else {
                            resultVect.emplace_back(Position(xPos, yPos));
                        }
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
        auto players = getTeam(player)->getAllPlayers();
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
            if(!p->isFined && p->position == position){
                return p;
            }
        }

        return {};
    }

    auto Environment::arePlayerInSameTeam(const std::shared_ptr<const Player>& p1, const std::shared_ptr<const Player>& p2) const -> bool {
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

    auto Environment::getAllFreeCells() -> std::vector<Position> {
        //@TODO optimization!!!
        std::vector<Position> ret;
        ret.reserve(193);
        for (const auto &cell : getAllValidCells()) {
            if (cellIsFree(cell)) {
                ret.emplace_back(cell);
            }
        }

        return ret;
    }

    auto Environment::isPlayerInOwnRestrictedZone(const std::shared_ptr<Player>& player) const -> bool {
        const auto cell = Environment::getCell(player->position);
        const auto side = conversions::idToSide(player->id);
        if (side == TeamSide::LEFT && (cell == Cell::RestrictedLeft || cell == Cell::GoalLeft)) {
            return true;
        }

        if (side == TeamSide::RIGHT && (cell == Cell::RestrictedRight || cell == Cell::GoalRight)) {
            return true;
        }

        return false;
    }
    auto Environment::isPlayerInOpponentRestrictedZone(const std::shared_ptr<Player>& player) const  -> bool {
        const auto cell = Environment::getCell(player->position);
        const auto side = conversions::idToSide(player->id);
        if (side == TeamSide::LEFT && (cell == Cell::RestrictedRight || cell == Cell::GoalRight)) {
            return true;
        }
        if (side == TeamSide::RIGHT && (cell == Cell::RestrictedLeft || cell == Cell::GoalLeft)) {
            return true;
        }
        return false;
    }

    auto Environment::getTeam(const std::shared_ptr<Player>& player) const -> std::shared_ptr<Team> {
        return getTeam(conversions::idToSide(player->id));
    }

    auto Environment::getTeam(TeamSide side) const -> std::shared_ptr<Team> {
        return team1->side == side ? team1 : team2;
    }

    void Environment::placePlayerOnRandomFreeCell(const std::shared_ptr<Player>& player) {
        std::vector<Position> possibleCells;
        possibleCells.reserve(82);
        const auto side = conversions::idToSide(player->id);
        for(const auto &cell : getAllValidCells()){
            if(getCell(cell) == Cell::Centre || getCell(cell) == Cell::GoalLeft){
                continue;
            }

            if(side == TeamSide::LEFT && cell.x < 8 && cellIsFree(cell)){
                possibleCells.push_back(cell);
            }

            if(side == TeamSide::RIGHT && cell.x > 8 && cellIsFree(cell)){
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

    auto Environment::getPlayerById(communication::messages::types::EntityId id) const -> std::shared_ptr<Player> {
        auto player = team1->getPlayerByID(id);
        if(player.has_value()){
            return player.value();
        }

        player = team2->getPlayerByID(id);
        if(player.has_value()){
            return player.value();
        }

        throw std::runtime_error("No player with specified in this match");
    }

    auto Environment::getBallByID(const communication::messages::types::EntityId &id) const -> std::shared_ptr<Ball> {
        if (this->quaffle->id == id) {
            return this->quaffle;
        }
        else if (this->snitch->id == id) {
            return this->snitch;
        }
        else if (this->bludgers[0]->id == id) {
            return this->bludgers[0];
        }
        else if (this->bludgers[1]->id == id) {
            return this->bludgers[1];
        }
        else {
            throw std::runtime_error("There is no matching Ball to the selected ID on the field!");
        }
    }

    auto Environment::isGoalCell(const Position &pos) -> bool {

        for (const Position &goalPos : getGoalsLeft()) {
            if (goalPos == pos) {
                return true;
            }
        }

        for (const Position &goalPos : getGoalsRight()) {
            if (goalPos == pos) {
                return true;
            }
        }
        return false;
    }

    void Environment::removeDeprecatedShit() {
        for (auto &shit : pileOfShit) {
            if (shit->spawnedThisRound) {
                shit->spawnedThisRound = false;
            } else {
                removeShitOnCell(shit->position);
            }
        }
    }

    void Environment::removeShitOnCell(const Position &position) {

        for (auto it = this->pileOfShit.begin(); it < this->pileOfShit.end();) {
            if ((*it)->position == position) {
                it = this->pileOfShit.erase(it);
            }
            else {
                it++;
            }
        }
    }

    auto Environment::isShitOnCell(const Position &position) const -> bool {
        for (const auto &shit : this->pileOfShit) {
            if (shit->position == position) {
                return true;
            }
        }
        return false;
    }

    auto Environment::clone() const -> std::shared_ptr<Environment> {
        auto newQuaf = std::make_shared<Quaffle>(*this->quaffle);
        auto newSnitch = std::make_shared<Snitch>(*this->snitch);
        auto newBludgers = std::array<std::shared_ptr<Bludger>, 2>{std::make_shared<Bludger>(*this->bludgers[0]),
                std::make_shared<Bludger>(*this->bludgers[0])};
        std::deque<std::shared_ptr<CubeOfShit>> newShit;
        for(const auto &shit : pileOfShit){
            newShit.emplace_back(std::make_shared<CubeOfShit>(*shit));
        }

        return std::make_shared<Environment>(this->config, this->team1->clone(), this->team2->clone(),
                newQuaf, newSnitch, newBludgers, newShit);
    }

    auto Environment::getAllEmptyCellsAround(const Position &position) const -> std::vector<Position> {
        std::vector<Position> ret;
        ret.reserve(8);
        for(int x = position.x - 1; x <= position.x + 1; x++){
            for(int y = position.y - 1; y <= position.y + 1; y++){
                Position curr(x, y);
                if(curr != position && getCell(curr) != Cell::OutOfBounds &&
                    cellIsFree(curr) && !isShitOnCell(curr)){
                    ret.emplace_back(curr);
                }
            }
        }

        return ret;
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

    Chaser::Chaser(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, broom, id) {}

    Keeper::Keeper(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, broom, id) {}

    Seeker::Seeker(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, broom, id) {}

    Beater::Beater(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
            Player(position, broom, id) {}


    // Team

    Team::Team(Seeker seeker, Keeper keeper, std::array<Beater, 2> beaters, std::array<Chaser, 3> chasers, int score, Fanblock fanblock, TeamSide side) :
               seeker(std::make_shared<Seeker>(std::move(seeker))), keeper(std::make_shared<Keeper>(std::move(keeper))), beaters{std::make_shared<Beater>(std::move(beaters[0])),
                       std::make_shared<Beater>(std::move(beaters[1]))}, chasers{std::make_shared<Chaser>(std::move(chasers[0])), std::make_shared<Chaser>(std::move(chasers[1])),
                               std::make_shared<Chaser>(std::move(chasers[2]))}, score(score), fanblock(std::move(fanblock)), side(side) {
        for(const auto &player : getAllPlayers()){
            if(conversions::idToSide(player->id) != this->side){
                throw std::invalid_argument("Player-IDs not matching team side");
            }
        }
    }

   Team::Team(const communication::messages::request::TeamConfig& tConf, communication::messages::request::TeamFormation tForm, TeamSide side) :
   seeker(std::make_shared<Seeker>(Position{tForm.getSeekerX(), tForm.getSeekerY()}, tConf.getSeeker().getBroom(), side == TeamSide::LEFT ?
   communication::messages::types::EntityId::LEFT_SEEKER : communication::messages::types::EntityId::RIGHT_SEEKER)),
   keeper(std::make_shared<Keeper>(Position{tForm.getKeeperX(), tForm.getKeeperY()}, tConf.getKeeper().getBroom(), side == TeamSide::LEFT ?
   communication::messages::types::EntityId::LEFT_KEEPER : communication::messages::types::EntityId::RIGHT_KEEPER)),
   beaters{std::make_shared<Beater>(Position{tForm.getBeater1X(), tForm.getBeater1Y()}, tConf.getBeater1().getBroom(), side == TeamSide::LEFT ?
            communication::messages::types::EntityId::LEFT_BEATER1 : communication::messages::types::EntityId::RIGHT_BEATER1),
            std::make_shared<Beater>(Position{tForm.getBeater2X(), tForm.getBeater2Y()}, tConf.getBeater2().getBroom(), side == TeamSide::LEFT ?
            communication::messages::types::EntityId ::LEFT_BEATER2 : communication::messages::types::EntityId::RIGHT_BEATER2)},
   chasers{std::make_shared<Chaser>(Position{tForm.getChaser1X(), tForm.getChaser1Y()}, tConf.getChaser1().getBroom(), side == TeamSide::LEFT ?
            communication::messages::types::EntityId::LEFT_CHASER1 : communication::messages::types::EntityId::RIGHT_CHASER1),
            std::make_shared<Chaser>(Position{tForm.getChaser2X(), tForm.getChaser2Y()}, tConf.getChaser2().getBroom(), side == TeamSide::LEFT ?
            communication::messages::types::EntityId::LEFT_CHASER2 : communication::messages::types::EntityId::RIGHT_CHASER2),
            std::make_shared<Chaser>(Position{tForm.getChaser3X(), tForm.getChaser3Y()}, tConf.getChaser3().getBroom(), side == TeamSide::LEFT ?
            communication::messages::types::EntityId::LEFT_CHASER3 : communication::messages::types::EntityId::RIGHT_CHASER3)},
   fanblock(tConf.getElfs(), tConf.getGoblins(), tConf.getTrolls(), tConf.getNifflers(), tConf.getWombats()), side(side){}


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

    bool Team::hasMember(const std::shared_ptr<const Player>& player) const {
        return conversions::idToSide(player->id) == side;
    }

    int Team::numberOfBannedMembers() const{
        int ret = 0;
        for(const auto &player :getAllPlayers()){
            if(player->isFined){
                ret++;
            }
        }

        return ret;
    }

    auto Team::getPlayerByID(communication::messages::types::EntityId id) const -> std::optional<std::shared_ptr<Player>> {
        for(const auto &player : getAllPlayers()){
            if(player->id == id){
                return player;
            }
        }

        return {};
    }

    auto Team::clone() const -> std::shared_ptr<Team> {
        return std::make_shared<Team>(*this->seeker, *this->keeper, std::array<Beater, 2>{*this->beaters[0], *this->beaters[1]},
                std::array<Chaser, 3>{*this->chasers[0], *this->chasers[1], *this->chasers[2]}, this->score, this->fanblock, this->side);
    }

    // Config

    Config::Config(unsigned int maxRounds, const FoulDetectionProbs &foulDetectionProbs,
                   const GameDynamicsProbs &gameDynamicsProbs, std::map<communication::messages::types::Broom, double> extraTurnProbs) :
            maxRounds(maxRounds), foulDetectionProbs(foulDetectionProbs), gameDynamicsProbs(gameDynamicsProbs),
            extraTurnProbs(std::move(extraTurnProbs)) {}

    double Config::getExtraTurnProb(communication::messages::types::Broom broom) const{
        return extraTurnProbs.at(broom);
    }

    //Willste mal nen richtig großen ... KONSTRUKTOR sehen? ;)
    Config::Config(const communication::messages::broadcast::MatchConfig &config) : maxRounds(config.getMaxRounds()),
        foulDetectionProbs{config.getProbFoulFlacking(), config.getProbFoulHaversacking(),
                            config.getProbFoulStooging(), config.getProbFoulBlatching(), config.getProbFoulSnitchnip(), config.getProbFoulElf(),
                            config.getProbFoulGoblin(), config.getProbFoulTroll(), config.getProbFoulSnitch(), config.getProbWombatPoo()},
        gameDynamicsProbs{config.getProbThrowSuccess(), config.getProbKnockOut(), config.getProbCatchSnitch(),
                          config.getProbCatchQuaffle(), config.getProbWrestQuaffle()}{
        using Broom = communication::messages::types::Broom;
        extraTurnProbs.emplace(Broom::CLEANSWEEP11, config.getProbExtraCleansweep());
        extraTurnProbs.emplace(Broom::COMET260, config.getProbExtraComet());
        extraTurnProbs.emplace(Broom::NIMBUS2001, config.getProbExtraNimbus());
        extraTurnProbs.emplace(Broom::FIREBOLT, config.getProbExtraFirebolt());
        extraTurnProbs.emplace(Broom::TINDERBLAST, config.getProbExtraTinderblast());
    }

    double Config::getFoulDetectionProb(Foul foul) const {
        switch (foul) {
            case Foul::None:
                return 0;
            case Foul::BlockGoal:
                return foulDetectionProbs.blockGoal;
            case Foul::ChargeGoal:
                return foulDetectionProbs.chargeGoal;
            case Foul::MultipleOffence:
                return foulDetectionProbs.multipleOffence;
            case Foul::Ramming:
                return foulDetectionProbs.ramming;
            case Foul::BlockSnitch:
                return foulDetectionProbs.blockSnitch;
            default:
                throw std::runtime_error("Fatal error, enum out of bounds");
        }
    }

    double Config::getFoulDetectionProb(InterferenceType interference) const {
        switch (interference) {
            case InterferenceType::RangedAttack:
                return foulDetectionProbs.rangedAttack;
            case InterferenceType::Teleport:
                return foulDetectionProbs.teleport;
            case InterferenceType::Impulse:
                return foulDetectionProbs.impulse;
            case InterferenceType::SnitchPush:
                return foulDetectionProbs.snitchPush;
            case InterferenceType::BlockCell:
                return foulDetectionProbs.blockCell;
            default:
                throw std::runtime_error("Fatal error, enum out of bounds");
        }
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

    Vector::Vector(const Position &p1, const Position &p2) {
        this->x = p2.x - p1.x;
        this->y = p2.y - p1.y;
    }

    double Vector::abs() const{
        return std::sqrt(x * x + y * y);
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

    Vector Vector::orthogonal() const{
        return Vector(this->y, -this->x);
    }

    Object::Object(const Position &position, communication::messages::types::EntityId id) : position(position), id(id){}

    CubeOfShit::CubeOfShit(const Position &target) : Object(target, communication::messages::types::EntityId::LEFT_WOMBAT){}
}
