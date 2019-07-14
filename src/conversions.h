/**
 * @file conversions.h
 * @author Tim
 * @date 18.06.19
 * @brief Decleration of multiple conversions functions.
 */

#ifndef SOPRAGAMELOGIC_CONVERSIONS_H
#define SOPRAGAMELOGIC_CONVERSIONS_H

#include "GameModel.h"

namespace gameLogic::conversions{
    /**
     * Converts a Foul to server type BanReason
     * @param foul
     * @return
     */
    auto foulToBanReason(gameModel::Foul foul) -> communication::messages::types::BanReason;

    /**
     * Converts InterferenceType to server type EntityId given a TeamSide
     * @param type Type to be converted
     * @param side Side of the Interference belonging to
     * @return
     */
    auto interferenceToId(gameModel::InterferenceType type, gameModel::TeamSide side) -> communication::messages::types::EntityId;

    /**
     * Checks if entity is a fan
     * @param id
     * @return true if entity is a fan, false otherwise
     */
    bool isFan(communication::messages::types::EntityId id);

    /**
     * Checks if entity is a ball
     * @param id
     * @return true if entity is a ball, false otherwise
     */
    bool isBall(communication::messages::types::EntityId id);

    /**
     * Checks if entity is a player
     * @param id
     * @return true if entity is a player, false otherwise
     */
    bool isPlayer(communication::messages::types::EntityId id);

    /**
     * Converts an EntityId to a TeamSide
     * @param id
     * @throws std::runtime_error if id is no Player or Fan
     * @return
     */
    auto idToSide(communication::messages::types::EntityId id) -> gameModel::TeamSide;

    /**
     * Converts EntityId to FanType
     * @param id
     * @throws std::runtime_error if id is no Fan
     * @return
     */
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
