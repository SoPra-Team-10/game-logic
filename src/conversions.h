//
// Created by timluchterhand on 18.06.19.
//

#ifndef SOPRAGAMELOGIC_CONVERSIONS_H
#define SOPRAGAMELOGIC_CONVERSIONS_H

#include "GameModel.h"

namespace gameLogic::conversions{
    auto foulToBanReason(gameModel::Foul foul) -> communication::messages::types::BanReason;
    auto interferenceToId(gameModel::InterferenceType type, gameModel::TeamSide side) -> communication::messages::types::EntityId;
    bool isFan(communication::messages::types::EntityId id);
    bool isBall(communication::messages::types::EntityId id);
    bool isPlayer(communication::messages::types::EntityId id);
    auto idToSide(communication::messages::types::EntityId id) -> gameModel::TeamSide;
    auto idToFantype(communication::messages::types::EntityId id) -> communication::messages::types::FanType;

    /**
     * Converts FanType to InterferenceType
     * @param fanType
     * @return
     */
    auto fanToInterference(communication::messages::types::FanType fanType) -> gameModel::InterferenceType;

    /**
     * Converts InterferenceType to FanType
     * @param type
     * @return
     */
    auto interferenceToFan(gameModel::InterferenceType type) -> communication::messages::types::FanType;
}

#endif //SOPRAGAMELOGIC_CONVERSIONS_H
