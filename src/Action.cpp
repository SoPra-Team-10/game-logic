#include <utility>
#include "Action.h"
#include "GameModel.h"
#define QUAFFLETHROW (((INSTANCE_OF(actor, gameModel::Chaser)) || (INSTANCE_OF(actor, gameModel::Keeper))) && (INSTANCE_OF(ball, gameModel::Quaffle)))
#define BLUDGERSHOT ((INSTANCE_OF(actor, gameModel::Beater)) && (INSTANCE_OF(ball, gameModel::Bludger)))

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
                    //Throw success
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
            if(gameModel::Environment::isGoalCell(ball->position) && playerOnLandingCell.has_value() &&
                !playerOnLandingCell.value()->knockedOut){
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

        env->removeShitOnCell(ball->position);

        return {shotRes, fouls};
    }

    auto Shot::successProb() const -> double {
        if (check() == ActionCheckResult::Impossible){
            return 0;
        }

        auto playerOnTarget = env->getPlayer(target);
        const bool playerNotInSameTeam = !env->arePlayerInSameTeam(actor, playerOnTarget.value());
        const bool targetIsGoal = gameModel::Environment::isGoalCell(target);
        const bool isAnyPlayerOnTarget = playerOnTarget.has_value();

        if(QUAFFLETHROW) {
            if(targetIsGoal && isAnyPlayerOnTarget && playerNotInSameTeam){
                //100% bounce off on goal
                return 0;
            }

            if(isAnyPlayerOnTarget && !(INSTANCE_OF(playerOnTarget.value(), gameModel::Chaser) ||
                                        INSTANCE_OF(playerOnTarget.value(), gameModel::Keeper))) {
                //Target player cannot hold ball -> 100% bounce off
                return 0;
            }

            //Prob for !interception * prob for !miss
            return std::pow(1 - env->config.gameDynamicsProbs.catchQuaffle, getInterceptionPositions().size()) *
                std::pow(env->config.gameDynamicsProbs.throwSuccess, getDistance(actor->position, target));
        } else if(BLUDGERSHOT) {
            if(isAnyPlayerOnTarget && !INSTANCE_OF(playerOnTarget.value(), gameModel::Beater)) {
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
            } else if(BLUDGERSHOT && getDistance(actor->position, target) <= 3){
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

        return res::Impossible;
    }

    auto Shot::getInterceptionPositions() const -> std::vector<gameModel::Position>{
        auto crossedCells = gameController::getAllCrossedCells(this->actor->position, target);
        std::vector<gameModel::Position> ret;
        for(const auto &player : env->getOpponents(actor)){
            for(const auto &cell : crossedCells){
                if(player->position == cell && !player->isFined && !player->knockedOut){
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

    auto Shot::executeAll() const -> std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> {
        if (check() == ActionCheckResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }

        if(QUAFFLETHROW) {
            return executeAllQuaffle();
        } else if(BLUDGERSHOT){
            return executeAllBludger();
        } else {
            throw std::runtime_error("Fatal Error! Illegal Shot!");
        }
    }

    auto Shot::executeAllQuaffle() const -> std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> {
        const std::shared_ptr<const gameModel::Environment> &localEnv = env;
        std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> ret;


        //Handle intercepting players
        auto interceptPoints = getInterceptionPositions();
        std::optional<const std::shared_ptr<const gameModel::Player>> playerOnTarget = localEnv->getPlayer(target);
        if(playerOnTarget.has_value() && !localEnv->arePlayerInSameTeam(playerOnTarget.value(), actor)) {
            interceptPoints.emplace_back(target);
        }

        for(unsigned long i = 0; i < interceptPoints.size(); i++) {
            auto const &interceptPos = interceptPoints[i];
            //baseProb for interception at i-th player
            double baseProb = std::pow(1 - localEnv->config.gameDynamicsProbs.catchQuaffle, i) *
                              localEnv->config.gameDynamicsProbs.catchQuaffle;
            const std::shared_ptr<const gameModel::Player> interceptingPlayer = localEnv->getPlayer(interceptPos).value();
            if(INSTANCE_OF(interceptingPlayer, const gameModel::Seeker) ||
               INSTANCE_OF(interceptingPlayer, const gameModel::Beater)) {
                //Bounce off
                emplaceEnvs(baseProb, localEnv->getAllFreeCellsAround(interceptPos), ret);
            } else {
                //Quaffle catch
                if(gameModel::Environment::isGoalCell(interceptPos)) {
                    emplaceEnvs(baseProb, localEnv->getAllFreeCellsAround(interceptPos), ret);
                } else {
                    auto newEnv = localEnv->clone();
                    newEnv->quaffle->position = interceptPos;
                    ret.emplace_back(newEnv, baseProb);
                }
            }
        }

        double noInterceptProb = std::pow(1 - localEnv->config.gameDynamicsProbs.catchQuaffle, interceptPoints.size());
        double throwSuccess = std::pow(localEnv->config.gameDynamicsProbs.throwSuccess, getDistance(actor->position, target));
        //Handle miss
        emplaceEnvs(noInterceptProb * (1 - throwSuccess), getAllLandingCells(), ret);

        //Handle success
        double success = noInterceptProb * throwSuccess;
        if(playerOnTarget.has_value() && (INSTANCE_OF(playerOnTarget.value(), const gameModel::Seeker) ||
                                          INSTANCE_OF(playerOnTarget.value(), const gameModel::Beater))) {
            emplaceEnvs(success, localEnv->getAllFreeCellsAround(target), ret);
        } else {
            emplaceEnvs(success, {target}, ret);
        }

        return ret;
    }

    auto Shot::executeAllBludger() const -> std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> {
        const std::shared_ptr<const gameModel::Environment> &localEnv = env;
        std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> ret;
        std::optional<const std::shared_ptr<const gameModel::Player>> playerOnTarget = localEnv->getPlayer(target);
        auto emplaceKnockoutEnv = [&ret, &playerOnTarget, &localEnv, this](double baseProb, const std::optional<gameModel::Position> &pos){
            auto knockoutEnv = localEnv->clone();
            knockoutEnv->getPlayerById(playerOnTarget.value()->id)->knockedOut = true;
            auto freeCells = knockoutEnv->getAllFreeCells();
            //non deterministic since branching factor would be way to high
            knockoutEnv->getBallByID(ball->id)->position = freeCells[rng(0, static_cast<int>(freeCells.size()) - 1)];
            if(pos.has_value()) {
                knockoutEnv->quaffle->position = pos.value();
                knockoutEnv->removeShitOnCell(pos.value());
            }

            ret.emplace_back(knockoutEnv, baseProb * localEnv->config.gameDynamicsProbs.knockOut);
        };

        auto emplaceFailEnv = [&ret, &localEnv, this](){
            auto failEnv = localEnv->clone();
            failEnv->getBallByID(ball->id)->position = target;
            ret.emplace_back(failEnv, 1 - localEnv->config.gameDynamicsProbs.knockOut);
        };

        if(playerOnTarget.has_value() && !INSTANCE_OF(playerOnTarget.value(), const gameModel::Beater)) {
            if(localEnv->quaffle->position == target) {
                auto landingCells = localEnv->getAllFreeCellsAround(target);

                if (landingCells.empty()) {
                    throw std::runtime_error("No landing cells for the quaffle found.");
                }

                double prob = 1.0 / landingCells.size();
                for(const auto &cell : landingCells) {
                    emplaceKnockoutEnv(prob, cell);
                }

                emplaceFailEnv();
            } else {
                emplaceKnockoutEnv(1, std::nullopt);
                emplaceFailEnv();
            }
        } else {
            auto newEnv = localEnv->clone();
            newEnv->getBallByID(ball->id)->position = target;
            newEnv->removeShitOnCell(target);
            ret.emplace_back(newEnv, 1);
        }

        return ret;
    }

    void Shot::emplaceEnvs(double baseProb, const std::vector<gameModel::Position> &newPoses,
                           std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> &envList) const{
            double prob = baseProb / newPoses.size();
            for(const auto &cell : newPoses) {
                bool handled = false;
                for (auto &outcome : envList) {
                    if (outcome.first->quaffle->position == cell) {
                        outcome.second += prob;
                        handled = true;
                        break;
                    }
                }

                if (handled) {
                    continue;
                }

                auto newEnv = env->clone();
                newEnv->quaffle->position = cell;
                newEnv->removeShitOnCell(cell);
                auto goalRes = goalCheck(cell);
                if(goalRes.has_value()) {
                    if(goalRes.value() == ActionResult::ScoreLeft) {
                        newEnv->team1->score += GOAL_POINTS;
                    } else if(goalRes.value() == ActionResult::ScoreRight) {
                        newEnv->team2->score += GOAL_POINTS;
                    }
                }

                envList.emplace_back(newEnv, prob);
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

        const bool positionIsTarget = this->env->snitch->position == this->target;
        const bool isSeeker = (bool) INSTANCE_OF(this->actor, gameModel::Seeker);
        const bool actionWasTriggered = actionTriggered(env->config.gameDynamicsProbs.catchSnitch);
        if(env->snitch->exists && positionIsTarget && isSeeker && actionWasTriggered) {

            actions.push_back(ActionResult::SnitchCatch);
            env->getTeam(actor)->score += SNITCH_POINTS;
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
            ret *= (1 - env->config.getFoulDetectionProb(foul));
        }

        return ret;
    }

    auto Move::checkForFoul() const -> std::vector<gameModel::Foul> {
        using Env = gameModel::Environment;
        std::vector<gameModel::Foul> resVect;
        // Ramming
        auto player = env->getPlayer(target);
        if (player.has_value() && !player.value()->isFined && !env->arePlayerInSameTeam(player.value(), this->actor)) {
            resVect.emplace_back(gameModel::Foul::Ramming);
        }

        // BlockGoal
        const bool isLeftGoal = Env::getCell(this->target) == gameModel::Cell::GoalLeft;
        const bool isRightGoal = Env::getCell(this->target) == gameModel::Cell::GoalRight;
        if ((env->team1->hasMember(this->actor) && isLeftGoal) ||
            (env->team2->hasMember(this->actor) && isRightGoal)) {
            resVect.emplace_back(gameModel::Foul::BlockGoal);
        }

        // BlockSnitch
        const bool actorIsNotSeeker = !INSTANCE_OF(actor, gameModel::Seeker);
        const bool targetIsSnitchPos = this->target == this->env->snitch->position;
        if (actorIsNotSeeker && targetIsSnitchPos && env->snitch->exists) {
            resVect.emplace_back(gameModel::Foul::BlockSnitch);
        }

        if (INSTANCE_OF(actor, gameModel::Chaser)) {
            const bool actorHadQuaffle = env->quaffle->position == this->actor->position;
            // ChargeGoal
            if ( actorHadQuaffle &&
                ((env->team1->hasMember(this->actor) && isRightGoal) ||
                (env->team2->hasMember(this->actor) && isLeftGoal))) {
                resVect.emplace_back(gameModel::Foul::ChargeGoal);
            }

            // MultipleOffence
            if(Env::getCell(actor->position) == gameModel::Cell::Standard){
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
        std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> {
        if (check() == ActionCheckResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }

        std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> ret;
        executePartially(ret, ActionState::MovePlayers);
        return ret;
    }

    void Move::executePartially(std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> &resList,
            ActionState state) const {
        switch(state) {
            case ActionState::MovePlayers: {
                if(env->getPlayer(target).has_value()){
                    //make actor "invisible"
                    actor->isFined = true;
                    auto freeCells = env->getAllFreeCellsAround(target);
                    if(env->quaffle->position == actor->position) {
                        freeCells.emplace_back(env->quaffle->position);
                    }
                    //make visible again
                    actor->isFined = false;
                    double prob = 1.0 / freeCells.size();
                    resList.reserve(freeCells.size());
                    for(const auto &pos : freeCells){
                        auto newEnv = env->clone();
                        //move target player on adjacent cell
                        newEnv->getPlayer(target).value()->position = pos;
                        //move actor on target
                        newEnv->getPlayerById(actor->id)->position = target;
                        newEnv->removeShitOnCell(pos);

                        resList.emplace_back(newEnv, prob);
                    }
                } else {
                    resList.emplace_back(env->clone(), 1);
                    resList.back().first->getPlayerById(actor->id)->position = target;
                }

                executePartially(resList, ActionState::HandleBalls);
            }
                break;
            case ActionState::HandleBalls: {
                std::optional<std::shared_ptr<const gameModel::Player>> playerOnTarget = env->getPlayer(target);
                //Handle quaffle
                if(env->quaffle->position == actor->position &&
                   (INSTANCE_OF(actor, gameModel::Chaser) || INSTANCE_OF(actor, gameModel::Keeper))) {
                    //Take Quaffle with actor
                    for(auto &outcome : resList) {
                        outcome.first->quaffle->position = target;
                        if(gameModel::Environment::getCell(target) == gameModel::Cell::GoalLeft) {
                            outcome.first->team2->score += GOAL_POINTS;
                        } else if(gameModel::Environment::getCell(target) == gameModel::Cell::GoalRight) {
                            outcome.first->team1->score += GOAL_POINTS;
                        }
                    }
                } else if(env->quaffle->position == target && (INSTANCE_OF(actor, gameModel::Beater) || INSTANCE_OF(actor, gameModel::Seeker) ||
                    (playerOnTarget.has_value() && !env->arePlayerInSameTeam(actor, playerOnTarget.value())))) {
                    std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> newOutcomes;
                    //Move Quaffle away from target
                    for(auto &outcome : resList) {
                        auto freeCells = env->getAllFreeCellsAround(target);
                        double prob = 1.0 / freeCells.size();
                        for(auto pos = freeCells.begin(); pos != freeCells.end(); pos++) {
                            if(pos == freeCells.begin()) {
                                //Alter in place
                                outcome.first->quaffle->position = *pos;
                                outcome.first->removeShitOnCell(*pos);

                                outcome.second *= prob;
                            } else {
                                //Create new envs
                                auto newEnv = outcome.first->clone();
                                newEnv->quaffle->position = *pos;
                                newEnv->removeShitOnCell(*pos);

                                newOutcomes.emplace_back(newEnv, outcome.second);
                            }
                        }
                    }

                    resList.insert(resList.end(), newOutcomes.begin(), newOutcomes.end());
                }

                //Handle snitch
                if(env->snitch->exists && env->snitch->position == target &&
                    INSTANCE_OF(actor, gameModel::Seeker)) {
                    std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> newOutcomes;
                    newOutcomes.reserve(resList.size() * 2);
                    for(auto &outcome : resList) {
                        auto catchFailEnv = outcome.first->clone();
                        auto catchFailBaseProb = outcome.second;
                        auto tempActorSucc = outcome.first->getPlayerById(actor->id);
                        outcome.first->getTeam(tempActorSucc)->score += SNITCH_POINTS;
                        outcome.second *= env->config.gameDynamicsProbs.catchSnitch;
                        newOutcomes.emplace_back(catchFailEnv, catchFailBaseProb * (1 - env->config.gameDynamicsProbs.catchSnitch));
                    }

                    resList.insert(resList.end(), newOutcomes.begin(), newOutcomes.end());
                }

                executePartially(resList, ActionState::HandleFouls);
            }
                break;
            case ActionState::HandleFouls: {
                double success = successProb();
                if(std::abs(1 - success) > std::numeric_limits<double>::epsilon()){
                    std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> newOutcomes;
                    resList.reserve(resList.size() * 2);
                    for(auto &outcome : resList){
                        auto newEnv = outcome.first->clone();
                        newEnv->getPlayerById(actor->id)->isFined = true;
                        newOutcomes.emplace_back(newEnv, outcome.second * (1 - success));
                        outcome.second *= success;
                    }

                    resList.insert(resList.end(), newOutcomes.begin(), newOutcomes.end());
                }
            }
                break;
        }
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

    auto WrestQuaffle::executeAll() const ->
        std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> {
        if(check() == ActionCheckResult::Impossible){
            throw std::runtime_error("Action is impossible");
        }

        std::vector<std::pair<std::shared_ptr<gameModel::Environment>, double>> ret;
        ret.reserve(2);
        ret.emplace_back(env->clone(), 1 - env->config.gameDynamicsProbs.wrestQuaffle);
        auto successEnv = env->clone();
        successEnv->quaffle->position = actor->position;
        ret.emplace_back(successEnv, env->config.gameDynamicsProbs.wrestQuaffle);
        return ret;

    }
}
