#include "GameModel.h"
#include "GameController.h"
#include "conversions.h"
#include "SharedPtrSerialization.h"

#include <utility>
#include <iostream>
#include <cmath>
#include <utility>

namespace gameModel{

    Player::Player(Position position, communication::messages::types::Broom broom, communication::messages::types::EntityId id) :
        Object(position, id), broom(broom), isFined{false} {
    }

    bool Player::operator==(const Player &other) const {
        auto tObject = static_cast<const Object*>(this);
        auto oObject = static_cast<const Object*>(&other);
        return *tObject == *oObject && broom == other.broom && this->knockedOut == other.knockedOut && this->isFined == other.isFined;
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
        return getUses(gameLogic::conversions::fanToInterference(fan));
    }

    int Fanblock::getBannedCount(gameModel::InterferenceType fan) const {
        return initialFans.at(fan) - currFans.at(fan);
    }

    int Fanblock::getBannedCount(communication::messages::types::FanType fan) const {
        return getBannedCount(gameLogic::conversions::fanToInterference(fan));
    }

    void Fanblock::banFan(InterferenceType fan) {
        if(--currFans.at(fan) < 0){
            throw std::runtime_error("No fans left to ban!");
        }
    }

    void Fanblock::banFan(communication::messages::types::FanType fan) {
        banFan(gameLogic::conversions::fanToInterference(fan));
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

    auto Environment::getTeamMates(const std::shared_ptr<const Player>& player) const -> std::array<std::shared_ptr<Player>, 6> {
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

    auto Environment::getOpponents(const std::shared_ptr<const Player>& player) const -> std::array<std::shared_ptr<Player>, 7> {
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

    auto Environment::getAllFreeCells() const -> std::vector<Position> {
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

    auto Environment::isPlayerInOwnRestrictedZone(const std::shared_ptr<const Player>& player) const -> bool {
        const auto cell = Environment::getCell(player->position);
        const auto side = gameLogic::conversions::idToSide(player->getId());
        if (side == TeamSide::LEFT && (cell == Cell::RestrictedLeft || cell == Cell::GoalLeft)) {
            return true;
        }

        if (side == TeamSide::RIGHT && (cell == Cell::RestrictedRight || cell == Cell::GoalRight)) {
            return true;
        }

        return false;
    }
    auto Environment::isPlayerInOpponentRestrictedZone(const std::shared_ptr<const Player>& player) const  -> bool {
        const auto cell = Environment::getCell(player->position);
        const auto side = gameLogic::conversions::idToSide(player->getId());
        if (side == TeamSide::LEFT && (cell == Cell::RestrictedRight || cell == Cell::GoalRight)) {
            return true;
        }
        if (side == TeamSide::RIGHT && (cell == Cell::RestrictedLeft || cell == Cell::GoalLeft)) {
            return true;
        }
        return false;
    }

    auto Environment::getTeam(const std::shared_ptr<const Player>& player) const -> std::shared_ptr<Team> {
        return getTeam(gameLogic::conversions::idToSide(player->getId()));
    }

    auto Environment::getTeam(TeamSide side) const -> std::shared_ptr<Team> {
        return team1->getSide() == side ? team1 : team2;
    }

    void Environment::placePlayerOnRandomFreeCell(const std::shared_ptr<Player>& player) {
        std::vector<Position> possibleCells;
        const auto side = gameLogic::conversions::idToSide(player->getId());
        possibleCells = getFreeCellsForRedeploy(side);
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
        if (this->quaffle->getId() == id) {
            return this->quaffle;
        }
        else if (this->snitch->getId() == id) {
            return this->snitch;
        }
        else if (this->bludgers[0]->getId() == id) {
            return this->bludgers[0];
        }
        else if (this->bludgers[1]->getId() == id) {
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
                std::make_shared<Bludger>(*this->bludgers[1])};
        std::deque<std::shared_ptr<CubeOfShit>> newShit;
        for(const auto &shit : pileOfShit){
            newShit.emplace_back(std::make_shared<CubeOfShit>(*shit));
        }

        return std::make_shared<Environment>(this->config, this->team1->clone(), this->team2->clone(),
                newQuaf, newSnitch, newBludgers, newShit);
    }

    auto Environment::getAllLegalCellsAround(const Position &position, bool leftTeam) const -> std::vector<Position> {
        std::vector<Position> ret;
        ret.reserve(8);
        for(int x = position.x - 1; x <= position.x + 1; x++){
            for(int y = position.y - 1; y <= position.y + 1; y++){
                Position curr(x, y);
                auto playerOnCell = getPlayer(curr);
                auto opponentTeam = leftTeam ? team2 : team1;
                bool ownGoalCell = leftTeam ? getCell(curr) == Cell::GoalLeft : getCell(curr) == Cell::GoalRight;
                if(curr != position && getCell(curr) != Cell::OutOfBounds && !isShitOnCell(curr) && !ownGoalCell &&
                    (!playerOnCell.has_value() || !opponentTeam->hasMember(playerOnCell.value()))){
                    ret.emplace_back(curr);
                }
            }
        }

        return ret;
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

    auto Environment::getFreeCellsForRedeploy(const gameModel::TeamSide &teamSide)const -> const std::vector<gameModel::Position> {
        std::vector<gameModel::Position> ret;
        ret.reserve(84);
        for(const auto &cell : this->getAllFreeCells()){
            if(teamSide == gameModel::TeamSide::LEFT){
                if(cell.x < FIELD_CENTRE_COL && this->getCell(cell) != gameModel::Cell::GoalLeft &&
                    this->cellIsFree(cell) && !this->isShitOnCell(cell) && this->getCell(cell) != gameModel::Cell::Centre){
                    ret.emplace_back(cell);
                }
            }else{
                if(cell.x > FIELD_CENTRE_COL && this->getCell(cell) != gameModel::Cell::GoalRight &&
                    this->cellIsFree(cell) && !this->isShitOnCell(cell) && this->getCell(cell) != gameModel::Cell::Centre){
                    ret.emplace_back(cell);
                }
            }
        }
        return ret;
    }

    bool Environment::operator==(const Environment &other) const {
        if(this->pileOfShit.size() != other.pileOfShit.size()){
            return false;
        }

        for(size_t i = 0; i < this->pileOfShit.size(); i++){
            if(*this->pileOfShit[i] != *other.pileOfShit[i]){
                return false;
            }
        }

        return *this->team1 == *other.team1 && *this->team2 == *other.team2 && *this->quaffle == *other.quaffle &&
            *this->snitch == *other.snitch && *this->bludgers[0] == *other.bludgers[0] && *this->bludgers[1] == *other.bludgers[1] &&
            this->config == other.config;
    }

    bool Environment::operator!=(const Environment &other) const {
        return !(*this == other);
    }


    // Ball Types

    Ball::Ball(Position position, communication::messages::types::EntityId id) : Object(position, id) {}

    Snitch::Snitch(Position position): Ball(position, communication::messages::types::EntityId::SNITCH) {}

    Snitch::Snitch() : Ball({0, 0}, communication::messages::types::EntityId::SNITCH), exists(false){
        auto allCells = Environment::getAllValidCells();
        auto index = gameController::rng(0, static_cast<int>(allCells.size()) - 1);
        position = allCells[index];
    }

    bool Snitch::operator==(const Snitch &other) const {
        auto tBall = static_cast<const Ball*>(this);
        auto oBall = static_cast<const Ball*>(&other);
        return (*tBall == *oBall) && this->exists == other.exists;
    }

    bool Snitch::operator!=(const Snitch &other) const {
        return !(*this == other);
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
            if(gameLogic::conversions::idToSide(player->getId()) != this->side){
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
        return gameLogic::conversions::idToSide(player->getId()) == side;
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
            if(player->getId() == id){
                return player;
            }
        }

        return {};
    }

    auto Team::clone() const -> std::shared_ptr<Team> {
        return std::make_shared<Team>(*this->seeker, *this->keeper, std::array<Beater, 2>{*this->beaters[0], *this->beaters[1]},
                std::array<Chaser, 3>{*this->chasers[0], *this->chasers[1], *this->chasers[2]}, this->score, this->fanblock, this->side);
    }

    TeamSide Team::getSide() const {
        return side;
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

    unsigned int Config::getMaxRounds() const {
        return maxRounds;
    }

    const GameDynamicsProbs &Config::getGameDynamicsProbs() const {
        return gameDynamicsProbs;
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

    communication::messages::types::EntityId Object::getId() const {
        return id;
    }

    CubeOfShit::CubeOfShit(const Position &target) : Object(target, communication::messages::types::EntityId::LEFT_WOMBAT){}

    bool CubeOfShit::operator==(const CubeOfShit &other) const {
        auto tObject = static_cast<const Object*>(this);
        auto oObject = static_cast<const Object*>(&other);
        return *tObject == *oObject && this->spawnedThisRound == other.spawnedThisRound;
    }

    bool CubeOfShit::operator!=(const CubeOfShit &other) const {
        return !(*this == other);
    }

    void to_json(nlohmann::json &j, const Position &position){
        j["x"] = position.x;
        j["y"] = position.y;
    }

    void to_json(nlohmann::json &j, const Object &object){
        j["position"] = object.position;
        j["id"] = object.getId();
    }

    void to_json(nlohmann::json &j, const Player &player){
        to_json(j, *static_cast<const Object*>(&player));
        j["fined"] = player.isFined;
        j["knockedOut"] = player.knockedOut;
        j["broom"] = player.broom;
    }

    void to_json(nlohmann::json &j, const Snitch &snitch){
        to_json(j, *static_cast<const Object*>(&snitch));
        j["exists"] = snitch.exists;
    }

    void to_json(nlohmann::json &j, const Fanblock &fanblock){
        j["teleports"] = fanblock.getUses(InterferenceType::Teleport);
        j["rangedAttacks"] = fanblock.getUses(InterferenceType::RangedAttack);
        j["impulses"] = fanblock.getUses(InterferenceType::Impulse);
        j["snitchPushes"] = fanblock.getUses(InterferenceType::SnitchPush);
        j["blockCells"] = fanblock.getUses(InterferenceType::BlockCell);

        j["teleportsBanned"] = fanblock.getBannedCount(InterferenceType::Teleport);
        j["rangedAttacksBanned"] = fanblock.getBannedCount(InterferenceType::RangedAttack);
        j["impulsesBanned"] = fanblock.getBannedCount(InterferenceType::Impulse);
        j["snitchPushesBanned"] = fanblock.getBannedCount(InterferenceType::SnitchPush);
        j["blockCellsBanned"] = fanblock.getBannedCount(InterferenceType::BlockCell);
    }

    void to_json(nlohmann::json &j, const Team &team){
        j["fanblock"] = team.fanblock;
        j["seeker"] = team.seeker;
        j["keeper"] = team.keeper;
        j["beater0"] = team.beaters[0];
        j["beater1"] = team.beaters[1];
        j["chaser0"] = team.chasers[0];
        j["chaser1"] = team.chasers[1];
        j["chaser2"] = team.chasers[2];
        j["score"] = team.score;
        j["side"] = team.getSide();
    }

    void to_json(nlohmann::json &j, const Config &config) {
        using namespace communication::messages::types;
        j["maxRounds"] = config.getMaxRounds();
        j["catchSnitchProb"] = config.getGameDynamicsProbs().catchSnitch;
        j["knockOutProb"] = config.getGameDynamicsProbs().knockOut;
        j["throwSuccessProb"] = config.getGameDynamicsProbs().throwSuccess;
        j["catchQuaffleProb"] = config.getGameDynamicsProbs().catchQuaffle;
        j["wrestQuaffleProb"] = config.getGameDynamicsProbs().wrestQuaffle;

        j["teleportFoulProb"] = config.getFoulDetectionProb(InterferenceType::Teleport);
        j["rangedAttackFoulProb"] = config.getFoulDetectionProb(InterferenceType::RangedAttack);
        j["impulseFoulProb"] = config.getFoulDetectionProb(InterferenceType::Impulse);
        j["snitchPushFoulProb"] = config.getFoulDetectionProb(InterferenceType::SnitchPush);
        j["blockCellFoulProb"] = config.getFoulDetectionProb(InterferenceType::BlockCell);

        j["multipleOffence"] = config.getFoulDetectionProb(Foul::MultipleOffence);
        j["ramming"] = config.getFoulDetectionProb(Foul::Ramming);
        j["blockGoal"] = config.getFoulDetectionProb(Foul::BlockGoal);
        j["blockSnitch"] = config.getFoulDetectionProb(Foul::BlockSnitch);
        j["chargeGoal"] = config.getFoulDetectionProb(Foul::ChargeGoal);

        j["tinderblastProb"] = config.getExtraTurnProb(Broom::TINDERBLAST);
        j["fireBoltProb"] = config.getExtraTurnProb(Broom::FIREBOLT);
        j["nimbusProb"] = config.getExtraTurnProb(Broom::NIMBUS2001);
        j["cometProb"] = config.getExtraTurnProb(Broom::COMET260);
        j["cleanSweepProb"] = config.getExtraTurnProb(Broom::CLEANSWEEP11);
    }

    void to_json(nlohmann::json &j, const Environment &environment){
        j["leftTeam"] = environment.getTeam(TeamSide::LEFT);
        j["rightTeam"] = environment.getTeam(TeamSide::RIGHT);
        j["snitch"] = environment.snitch;
        j["quaffle"] = environment.quaffle;
        j["bludger0"] = environment.bludgers[0];
        j["bludger1"] = environment.bludgers[1];
        j["config"] = environment.config;
        j["pileOfShit"] = environment.pileOfShit;
    }

    void from_json(const nlohmann::json &j, Position &position) {
        position.x = j.at("x").get<int>();
        position.y = j.at("y").get<int>();
    }

    void from_json(const nlohmann::json &j, Object &object) {
        object.position = j.at("position").get<Position>();
        object.id = j.at("id").get<communication::messages::types::EntityId>();
    }

    bool Object::operator==(const Object &other) const {
        return this->position == other.position && this->getId() == other.getId();
    }

    bool Object::operator!=(const Object &other) const {
        return !(*this == other);
    }

    void from_json(const nlohmann::json &j, Player &player) {
        from_json(j, *static_cast<Object*>(&player));
        player.broom = j.at("broom").get<communication::messages::types::Broom>();
        player.isFined = j.at("fined").get<bool>();
        player.knockedOut = j.at("knockedOut").get<bool>();
    }

    void from_json(const nlohmann::json &j, Snitch &snitch) {
        from_json(j, *static_cast<Object*>(&snitch));
        snitch.exists = j.at("exists").get<bool>();
    }

    void from_json(const nlohmann::json &j, Fanblock &fanblock) {
        fanblock.initialFans.clear();
        fanblock.currFans.clear();
        auto tel = j.at("teleports").get<int>();
        auto ra = j.at("rangedAttacks").get<int>();
        auto imp = j.at("impulses").get<int>();
        auto sp = j.at("snitchPushes").get<int>();
        auto bc = j.at("blockCells").get<int>();

        auto telB = j.at("teleportsBanned").get<int>();
        auto raB = j.at("rangedAttacksBanned").get<int>();
        auto impB = j.at("impulsesBanned").get<int>();
        auto spB = j.at("snitchPushesBanned").get<int>();
        auto bcB = j.at("blockCellsBanned").get<int>();

        fanblock.currFans.emplace(InterferenceType::Teleport, tel);
        fanblock.currFans.emplace(InterferenceType::RangedAttack, ra);
        fanblock.currFans.emplace(InterferenceType::Impulse, imp);
        fanblock.currFans.emplace(InterferenceType::SnitchPush, sp);
        fanblock.currFans.emplace(InterferenceType::BlockCell, bc);

        fanblock.initialFans.emplace(InterferenceType::Teleport, tel + telB);
        fanblock.initialFans.emplace(InterferenceType::RangedAttack, ra + raB);
        fanblock.initialFans.emplace(InterferenceType::Impulse, imp + impB);
        fanblock.initialFans.emplace(InterferenceType::SnitchPush, sp + spB);
        fanblock.initialFans.emplace(InterferenceType::BlockCell, bc + bcB);
    }

    bool Fanblock::operator==(const Fanblock &other) const {
        return initialFans == other.initialFans && currFans == other.currFans;
    }

    bool Fanblock::operator!=(const Fanblock &other) const {
        return !(*this == other);
    }

    void from_json(const nlohmann::json &j, Team &team) {
        team.fanblock = j.at("fanblock").get<Fanblock>();
        team.keeper = j.at("keeper").get<std::shared_ptr<Keeper>>();
        team.seeker = j.at("seeker").get<std::shared_ptr<Seeker>>();
        team.beaters = {j.at("beater0").get<std::shared_ptr<Beater>>(), j.at("beater1").get<std::shared_ptr<Beater>>()};
        team.chasers = {j.at("chaser0").get<std::shared_ptr<Chaser>>(), j.at("chaser1").get<std::shared_ptr<Chaser>>(),
                        j.at("chaser2").get<std::shared_ptr<Chaser>>()};
        team.side = j.at("side").get<TeamSide>();
        team.score = j.at("score").get<int>();
    }

    bool Team::operator!=(const Team &other) const {
        return !(*this == other);
    }

    bool Team::operator==(const Team &other) const {
        auto tPlayers = getAllPlayers();
        auto oPlayers = other.getAllPlayers();
        for(size_t i = 0; i < tPlayers.size(); i++){
            if(*tPlayers[i] != *oPlayers[i]){
                return false;
            }
        }

        return this->getSide() == other.getSide() && this->fanblock == other.fanblock && this->score == other.score;
    }

    void from_json(const nlohmann::json &j, Config &config) {
        using namespace communication::messages::types;
        config.maxRounds = j.at("maxRounds").get<unsigned int>();
        config.gameDynamicsProbs.wrestQuaffle = j.at("wrestQuaffleProb").get<double>();
        config.gameDynamicsProbs.catchQuaffle = j.at("catchQuaffleProb").get<double>();
        config.gameDynamicsProbs.throwSuccess = j.at("throwSuccessProb").get<double>();
        config.gameDynamicsProbs.knockOut = j.at("knockOutProb").get<double>();
        config.gameDynamicsProbs.catchSnitch = j.at("catchSnitchProb").get<double>();

        config.foulDetectionProbs.teleport = j.at("teleportFoulProb").get<double>();
        config.foulDetectionProbs.impulse = j.at("impulseFoulProb").get<double>();
        config.foulDetectionProbs.snitchPush = j.at("snitchPushFoulProb").get<double>();
        config.foulDetectionProbs.rangedAttack = j.at("rangedAttackFoulProb").get<double>();
        config.foulDetectionProbs.blockCell = j.at("blockCellFoulProb").get<double>();

        config.foulDetectionProbs.blockGoal = j.at("blockGoal").get<double>();
        config.foulDetectionProbs.blockSnitch = j.at("blockSnitch").get<double>();
        config.foulDetectionProbs.chargeGoal = j.at("chargeGoal").get<double>();
        config.foulDetectionProbs.multipleOffence = j.at("multipleOffence").get<double>();
        config.foulDetectionProbs.ramming = j.at("ramming").get<double>();

        config.extraTurnProbs.clear();
        config.extraTurnProbs.emplace(Broom::CLEANSWEEP11, j.at("cleanSweepProb").get<double>());
        config.extraTurnProbs.emplace(Broom::COMET260, j.at("cometProb").get<double>());
        config.extraTurnProbs.emplace(Broom::NIMBUS2001, j.at("nimbusProb").get<double>());
        config.extraTurnProbs.emplace(Broom::FIREBOLT, j.at("fireBoltProb").get<double>());
        config.extraTurnProbs.emplace(Broom::TINDERBLAST, j.at("tinderblastProb").get<double>());
    }

    bool Config::operator==(const Config &rhs) const {
        return maxRounds == rhs.maxRounds &&
               foulDetectionProbs == rhs.foulDetectionProbs &&
               gameDynamicsProbs == rhs.gameDynamicsProbs &&
               extraTurnProbs == rhs.extraTurnProbs;
    }

    bool Config::operator!=(const Config &rhs) const {
        return !(rhs == *this);
    }

    void from_json(const nlohmann::json &j, Environment &environment) {
        environment.team1 = j.at("leftTeam").get<std::shared_ptr<Team>>();
        environment.team2 = j.at("rightTeam").get<std::shared_ptr<Team>>();
        environment.config = j.at("config").get<Config>();
        environment.quaffle = j.at("quaffle").get<std::shared_ptr<Quaffle>>();
        environment.snitch = j.at("snitch").get<std::shared_ptr<Snitch>>();
        environment.bludgers = {j.at("bludger0").get<std::shared_ptr<Bludger>>(), j.at("bludger1").get<std::shared_ptr<Bludger>>()};
        environment.pileOfShit = j.at("pileOfShit").get<std::deque<std::shared_ptr<CubeOfShit>>>();
    }

    bool FoulDetectionProbs::operator==(const FoulDetectionProbs &rhs) const {
        return blockGoal == rhs.blockGoal &&
               chargeGoal == rhs.chargeGoal &&
               multipleOffence == rhs.multipleOffence &&
               ramming == rhs.ramming &&
               blockSnitch == rhs.blockSnitch &&
               teleport == rhs.teleport &&
               rangedAttack == rhs.rangedAttack &&
               impulse == rhs.impulse &&
               snitchPush == rhs.snitchPush &&
               blockCell == rhs.blockCell;
    }

    bool FoulDetectionProbs::operator!=(const FoulDetectionProbs &rhs) const {
        return !(rhs == *this);
    }

    bool GameDynamicsProbs::operator==(const GameDynamicsProbs &rhs) const {
        return throwSuccess == rhs.throwSuccess &&
               knockOut == rhs.knockOut &&
               catchSnitch == rhs.catchSnitch &&
               catchQuaffle == rhs.catchQuaffle &&
               wrestQuaffle == rhs.wrestQuaffle;
    }

    bool GameDynamicsProbs::operator!=(const GameDynamicsProbs &rhs) const {
        return !(rhs == *this);
    }
}
