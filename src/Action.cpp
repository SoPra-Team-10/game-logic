#include <utility>
#include "Action.h"
#include "GameModel.h"
#define QUAFFLETHROW ((INSTANCE_OF(actor, gameModel::Chaser) || INSTANCE_OF(actor, gameModel::Keeper)) && INSTANCE_OF(ball, gameModel::Quaffle))
#define BLUDGERSHOT (INSTANCE_OF(actor, gameModel::Beater) && INSTANCE_OF(ball, gameModel::Bludger))

namespace gameController{
    Action::Action(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
            gameModel::Position target) :  actor(std::move(actor)), env(std::move(env)), target(target){}


    Shot::Shot(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor,
            std::shared_ptr<gameModel::Ball> ball, gameModel::Position target) :
            Action(std::move(env), std::move(actor), target), ball(std::move(ball)) {}

    auto Shot::execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>> {
        std::vector<gameModel::Foul> fouls;
        std::vector<ActionResult> shotRes;
        if (check() == ActionCheckResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }

        if(QUAFFLETHROW){
            bool intercepted = false;
            auto playerOnTarget = env->getPlayer(target);
            auto positionToCheck = getInterceptionPositions();
            if (playerOnTarget.has_value() && !playerOnTarget.value()->isFined &&
                !env->arePlayerInSameTeam(actor, playerOnTarget.value())) {
                positionToCheck.emplace_back(target);
            }

            for(const auto &pos : positionToCheck) {
                auto interceptPlayer = env->getPlayer(pos);
                if(!interceptPlayer.has_value()){
                    throw std::runtime_error("No Player at intercept position");
                }

                if(actionTriggered(env->config.gameDynamicsProbs.catchQuaffle)) {
                    intercepted = true;
                    ball->position = pos;
                    if(gameModel::Environment::isGoalCell(pos)){
                        //intercepted on goal
                        moveToAdjacent(ball, env);
                    } else {
                        //normal intercept
                        auto &p = interceptPlayer.value();
                        if (!INSTANCE_OF(p, gameModel::Chaser) && !INSTANCE_OF(p, gameModel::Keeper)) {
                            //Quaffle bounces off
                            moveToAdjacent(ball, env);
                        }
                    }

                    break;
                }
            }

            //Quaffle not intercepted
            if(!intercepted) {
                auto dist = getDistance(actor->position, target);
                if(actionTriggered(std::pow(env->config.gameDynamicsProbs.throwSuccess, dist))){
                    //Throw successs
                    ball->position = target;
                } else {
                    //Miss -> dispersion
                    auto possibleCells = getAllLandingCells();
                    if(!possibleCells.empty()){
                        int index = rng(0, static_cast<int>(possibleCells.size()) - 1);
                        ball->position = possibleCells[index];
                    } else {
                        ball->position = target;
                    }
                }
            }

            auto playerOnLandingCell = env->getPlayer(ball->position);
            //Ball landet on a goal blocked by any player
            if(gameModel::Environment::isGoalCell(ball->position) && playerOnLandingCell.has_value()){
                moveToAdjacent(ball, env);
                intercepted = true;
            }

            if(target == ball->position){
                shotRes.push_back(ActionResult::ThrowSuccess);
            } else if(intercepted){
                shotRes.push_back(ActionResult::Intercepted);
            } else {
                shotRes.push_back(ActionResult::Miss);
            }

            auto goalCheckResult = goalCheck(ball->position);
            if(goalCheckResult.has_value()){
                shotRes.push_back(goalCheckResult.value());
                if(goalCheckResult.value() == ActionResult::ScoreLeft){
                    env->team1->score += GOAL_POINTS;
                } else if(goalCheckResult.value() == ActionResult::ScoreRight){
                    env->team2->score += GOAL_POINTS;
                } else {
                    throw std::runtime_error("Unexpected goal check result");
                }
            }

        } else if(BLUDGERSHOT){
            auto playerOnTarget = env->getPlayer(target);
            if(playerOnTarget.has_value()){
                //Knock player out an place bludger on random free cell
                if(!INSTANCE_OF(playerOnTarget.value(), gameModel::Beater) &&
                    actionTriggered(env->config.gameDynamicsProbs.knockOut)){
                    shotRes.push_back(ActionResult::Knockout);
                    playerOnTarget.value()->knockedOut = true;
                    auto possibleCells = env->getAllFreeCells();
                    int index = rng(0, static_cast<int>(possibleCells.size()) - 1);
                    ball->position = possibleCells[index];

                    //fool quaffle away
                    if(env->quaffle->position == target){
                        moveToAdjacent(env->quaffle, env);
                        shotRes.push_back(ActionResult::FoolAway);
                    }
                } else {
                    ball->position = target;
                }
            } else {
                ball->position = target;
            }
        }

        if(env->isShitOnCell(ball->position)){
            env->removeShitOnCell(ball->position);
        }

        return {shotRes, fouls};
    }

    auto Shot::successProb() const -> double {
        if (check() == ActionCheckResult::Impossible){
            return 0;
        }

        auto playerOnTarget = env->getPlayer(target);
        if(QUAFFLETHROW) {
            if(env->isGoalCell(target) && playerOnTarget.has_value() && !env->arePlayerInSameTeam(actor, playerOnTarget.value())){
                //100% bounce off on goal
                return 0;
            }

            if(playerOnTarget.has_value() && !(INSTANCE_OF(playerOnTarget.value(), gameModel::Chaser) ||
                                               INSTANCE_OF(playerOnTarget.value(), gameModel::Keeper))) {
                //Target player cannot hold ball -> 100% bounce off
                return 0;
            }

            //Prob for !interception * prob for !miss
            return std::pow(env->config.gameDynamicsProbs.catchQuaffle, getInterceptionPositions().size()) *
                std::pow(env->config.gameDynamicsProbs.throwSuccess, getDistance(actor->position, target));
        } else if(BLUDGERSHOT) {
            if(playerOnTarget.has_value() && !INSTANCE_OF(playerOnTarget.value(), gameModel::Beater)) {
                //Prob for knockout
                return env->config.gameDynamicsProbs.knockOut;
            } else {
                return 0;
            }
        } else {
            return 0;
        }
    }

    auto Shot::check() const -> ActionCheckResult {
        using res = ActionCheckResult;
        if(actor->position == ball->position && gameModel::Environment::getCell(target) != gameModel::Cell::OutOfBounds &&
            !actor->isFined && !actor->knockedOut){
            if(QUAFFLETHROW){
                return res::Success;
            } else if(BLUDGERSHOT){
                if(getDistance(actor->position, target) <= 3){
                    bool blocked = false;
                    for(const auto &cell : getAllCrossedCells(actor->position, target)){
                        if(env->getPlayer(cell).has_value()){
                            blocked = true;
                            break;
                        }
                    }

                    if(!blocked){
                        return res::Success;
                    }
                }
            }
        }

        return res::Impossible;
    }

    auto Shot::executeAll() const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        // @toDo: da fehlt noch alles

        return resultVect;
    }

    auto Shot::getInterceptionPositions() const -> std::vector<gameModel::Position>{
        auto crossedCells = gameController::getAllCrossedCells(this->actor->position, target);
        std::vector<gameModel::Position> ret;
        for(const auto &player : env->getOpponents(actor)){
            for(const auto &cell : crossedCells){
                if(player->position == cell){
                    ret.emplace_back(cell);
                }
            }
        }

        return ret;
    }

    auto Shot::getAllLandingCells() const -> std::vector<gameModel::Position> {
        int n = static_cast<int>(std::ceil(getDistance(actor->position, target) / 7.0));
        std::vector<gameModel::Position> ret;
        using Env = gameModel::Environment;
        using Cell = gameModel::Cell;
        ret.reserve((2 * n + 1) * (2 * n + 1) -1);

        int startX = target.x - n;
        int endX = target.x + n;
        int startY = target.y - n;
        int endY = target.y + n;

        do {
            for(int yPos = startY; yPos <= endY; yPos++){
                for(int xPos = startX; xPos <= endX; xPos++){

                    if(gameModel::Position{xPos, yPos} == target){
                        continue;
                    }

                    if(Env::getCell(xPos, yPos) == Cell::OutOfBounds){
                        continue;
                    }

                    if(env->cellIsFree({xPos, yPos})){
                        ret.emplace_back(xPos, yPos);
                    }
                }
            }

            startX--;
            endX++;
            startY--;
            endY++;
        } while (ret.empty());

        return ret;
    }

    auto Shot::goalCheck(const gameModel::Position &pos) const -> std::optional<ActionResult> {
        // senkrechter wurf
        if(actor->position.x == pos.x){
            return std::nullopt;
        }

        //Tor nicht getroffen
        if(!gameModel::Environment::isGoalCell(pos)){
            return std::nullopt;
        }

        double m = (pos.y - actor->position.y) / static_cast<double>((pos.x - actor->position.x));
        double c = actor->position.y - m * actor->position.x;
        auto left = std::min(actor->position.x, pos.x);
        auto right = std::max(actor->position.x, pos.x);
        bool score = false;
        if(pos.x >= left && pos.x <= right){
            auto upper = pos.y + 0.5;
            auto lower = pos.y - 0.5;
            auto lSide = m * (pos.x - 0.5) + c;
            auto rSide = m * (pos.x + 0.5) + c;
            score = (lSide > lower && lSide < upper) || (rSide > lower && rSide < upper);
        }

        if(score){
            return gameModel::Environment::getCell(pos) == gameModel::Cell::GoalLeft ?
                ActionResult::ScoreRight : ActionResult::ScoreLeft;
        } else {
            return std::nullopt;
        }
    }

    Move::Move(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Player> actor, gameModel::Position target):
            Action(std::move(env), std::move(actor), target) {}

    auto Move::check() const -> ActionCheckResult{

        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1 ||
            this->env->isShitOnCell(this->target) ||
            this->actor->isFined || this->actor->knockedOut){
            return ActionCheckResult::Impossible;
        }

        if (this->checkForFoul().empty()) {
            return ActionCheckResult::Success;
        }
        else {
            return ActionCheckResult::Foul;
        }
    }

    auto Move::execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>> {
        std::vector<gameModel::Foul> fouls;
        std::vector<ActionResult> actions;

        // check move
        bool rammingFoulFlag = false;
        ActionCheckResult actionResult = this->check();

        if (actionResult == ActionCheckResult::Impossible) {
            throw std::runtime_error("The selected move is impossible!");
        }
        else if (actionResult == ActionCheckResult::Foul) {
            for (const auto &foul :  this->checkForFoul()) {
                if (foul == gameModel::Foul::Ramming) {
                    rammingFoulFlag = true;
                }

                if (gameController::refereeDecision(foul, this->env->config)) {
                    fouls.emplace_back(foul);
                    this->actor->isFined = true;
                }
            }
        }

        if(env->snitch->exists) {
            if (this->env->snitch->position == this->target) {
                if (INSTANCE_OF(this->actor, gameModel::Seeker)) {
                    if (actionTriggered(env->config.gameDynamicsProbs.catchSnitch)) {
                        actions.push_back(ActionResult::SnitchCatch);
                        env->getTeam(actor)->score += SNITCH_POINTS;
                    }
                }
            }
        }

        // get other player on target cell
        auto targetPlayer = env->getPlayer(this->target);

        // move the player
        auto oldActorPos = actor->position;
        this->actor->position = this->target;

        // move the quaffel if necessary
        if (this->env->quaffle->position == oldActorPos) {
            this->env->quaffle->position = this->target;
            if (gameModel::Environment::isGoalCell(this->env->quaffle->position)) {
                if (gameModel::Environment::getCell(this->env->quaffle->position) == gameModel::Cell::GoalLeft) {
                    actions.push_back(ActionResult::ScoreRight);
                    env->team2->score += GOAL_POINTS;
                } else if (gameModel::Environment::getCell(this->env->quaffle->position) == gameModel::Cell::GoalRight) {
                    actions.push_back(ActionResult::ScoreLeft);
                    env->team1->score += GOAL_POINTS;
                }
            }
        } else if(rammingFoulFlag && env->quaffle->position == target) {
            //rammed player looses quaffel
            moveToAdjacent(env->quaffle, env);
            actions.push_back(ActionResult::FoolAway);
        } else if(this->env->quaffle->position == target && !INSTANCE_OF(this->actor, gameModel::Chaser) && !INSTANCE_OF(this->actor, gameModel::Keeper)){
            moveToAdjacent(env->quaffle, env);
        }

        // move other player out of the way if necessary
        if (targetPlayer.has_value() && !targetPlayer.value()->isFined) {
            gameController::moveToAdjacent(targetPlayer.value(), env);
        }

        return {actions, fouls};
    }

    auto Move::successProb() const -> double {
        if(check() == ActionCheckResult::Impossible){
            return 0;
        }

        double ret = 1;
        for(const auto &foul : checkForFoul()){
            ret *= env->config.getFoulDetectionProb(foul);
        }

        return ret;
    }

    auto Move::checkForFoul() const -> std::vector<gameModel::Foul> {
        std::vector<gameModel::Foul> resVect;
        // Ramming
        auto player = env->getPlayer(target);
        if (player.has_value() && !player.value()->isFined && !env->arePlayerInSameTeam(player.value(), this->actor)) {
            resVect.emplace_back(gameModel::Foul::Ramming);
        }

        // BlockGoal
        if ((env->team1->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalLeft) ||
            (env->team2->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalRight)) {
            resVect.emplace_back(gameModel::Foul::BlockGoal);
        }

        // BlockSnitch
        if (!INSTANCE_OF(actor, gameModel::Seeker) && this->target == this->env->snitch->position &&
                env->snitch->exists) {
            resVect.emplace_back(gameModel::Foul::BlockSnitch);
        }

        if (INSTANCE_OF(actor, gameModel::Chaser)) {

            // ChargeGoal
            if (env->quaffle->position == this->actor->position) {
                if ((env->team1->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalRight) ||
                    (env->team2->hasMember(this->actor) && env->getCell(this->target) == gameModel::Cell::GoalLeft)) {
                    resVect.emplace_back(gameModel::Foul::ChargeGoal);
                }
            }

            // MultipleOffence
            if(gameModel::Environment::getCell(actor->position) == gameModel::Cell::Standard){
                auto origPos = actor->position;
                actor->position = target;
                if(env->isPlayerInOpponentRestrictedZone(actor)){
                    auto mates = env->getTeamMates(actor);
                    for(const auto &p : mates){
                        if(!p->isFined && INSTANCE_OF(p, gameModel::Chaser) && env->isPlayerInOpponentRestrictedZone(p)){
                            resVect.emplace_back(gameModel::Foul::MultipleOffence);
                        }
                    }
                }

                actor->position = origPos;
            }
        }

        return resVect;
    }

    auto Move::executeAll() const ->
    std::vector<std::pair<gameModel::Environment, double>> {

        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        // @toDo: da fehlt noch alles

        return resultVect;
    }

    WrestQuaffle::WrestQuaffle(std::shared_ptr<gameModel::Environment> env, std::shared_ptr<gameModel::Chaser> actor,
                                 gameModel::Position target):
            Action(std::move(env), std::move(actor), target) {}

    auto WrestQuaffle::execute() const -> std::pair<std::vector<ActionResult>, std::vector<gameModel::Foul>> {
        std::vector<gameModel::Foul> fouls;
        std::vector<ActionResult> actions;

        // check snatch
        ActionCheckResult actionResult = this->check();
        if (actionResult == ActionCheckResult::Impossible) {
            throw std::runtime_error("The selected quaffel wresting is impossible!");
        }

        // wrest the quaffel
        if(actionTriggered(env->config.gameDynamicsProbs.wrestQuaffle)) {
            env->quaffle->position =  actor->position;
            actions.emplace_back(ActionResult::WrestQuaffel);
        }

        return {actions, fouls};
    }

    auto WrestQuaffle::successProb() const -> double {
        if (this->check() == ActionCheckResult::Success) {
            return env->config.gameDynamicsProbs.wrestQuaffle;
        }
        else {
            return 0;
        }
    }

    auto WrestQuaffle::check() const -> ActionCheckResult {
        auto player = env->getPlayer(target);
        if (gameModel::Environment::getCell(this->target) == gameModel::Cell::OutOfBounds ||
            gameController::getDistance(this->actor->position, this->target) > 1 ||
            this->actor->isFined || this->actor->knockedOut ||
            env->quaffle->position != target || !player.has_value() || player.value()->isFined) {
            return ActionCheckResult::Impossible;
        }

        if (INSTANCE_OF(player.value(), gameModel::Chaser)) {
            return ActionCheckResult::Success;
        }
        else if (INSTANCE_OF(player.value(), gameModel::Keeper) && !env->isPlayerInOwnRestrictedZone(player.value())){
            return ActionCheckResult::Success;
        }
        else {
            return ActionCheckResult::Impossible;
        }
    }
    // not now
    auto WrestQuaffle::executeAll() const -> std::vector<std::pair<gameModel::Environment, double>> {
        std::vector<std::pair<gameModel::Environment, double>> resultVect;

        // @toDo: da fehlt noch alles

        return resultVect;
    }
}
