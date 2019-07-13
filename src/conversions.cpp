/**
 * @file conversions.cpp
 * @author Tim
 * @date 18.06.19
 * @brief Implementation of multiple conversions functions.
 */

#include "conversions.h"

namespace gameLogic::conversions {

    auto foulToBanReason(gameModel::Foul foul) -> communication::messages::types::BanReason {
        using Reason = communication::messages::types::BanReason;
        switch (foul) {
            case gameModel::Foul::None:
                throw std::runtime_error("cannot gameLogic::convert");
            case gameModel::Foul::BlockGoal:
                return Reason::FLACKING;
            case gameModel::Foul::ChargeGoal:
                return Reason::HAVERSACKING;
            case gameModel::Foul::MultipleOffence:
                return Reason::STOOGING;
            case gameModel::Foul::Ramming:
                return Reason::BLATCHING;
            case gameModel::Foul::BlockSnitch:
                return Reason::SNITCHNIP;
        }

        throw std::runtime_error("Fatal error! Enum out of bounds");
    }

    auto interferenceToId(gameModel::InterferenceType type,
                                                  gameModel::TeamSide side) -> communication::messages::types::EntityId {
        using Id = communication::messages::types::EntityId;
        switch (type) {
            case gameModel::InterferenceType::RangedAttack:
                return side == gameModel::TeamSide::LEFT ? Id::LEFT_GOBLIN : Id::RIGHT_GOBLIN;
            case gameModel::InterferenceType::Teleport:
                return side == gameModel::TeamSide::LEFT ? Id::LEFT_ELF : Id::RIGHT_ELF;
            case gameModel::InterferenceType::Impulse:
                return side == gameModel::TeamSide::LEFT ? Id::LEFT_TROLL : Id::RIGHT_TROLL;
            case gameModel::InterferenceType::SnitchPush:
                return side == gameModel::TeamSide::LEFT ? Id::LEFT_NIFFLER : Id::RIGHT_NIFFLER;
            case gameModel::InterferenceType::BlockCell:
                return side == gameModel::TeamSide::LEFT ? Id::LEFT_WOMBAT : Id::RIGHT_WOMBAT;
        }

        throw std::runtime_error("Fatal error! Enum out of bounds");
    }

    bool isFan(communication::messages::types::EntityId id) {
        switch (id) {
            case communication::messages::types::EntityId::LEFT_GOBLIN:
            case communication::messages::types::EntityId::LEFT_TROLL:
            case communication::messages::types::EntityId::LEFT_ELF:
            case communication::messages::types::EntityId::LEFT_NIFFLER:
            case communication::messages::types::EntityId::RIGHT_GOBLIN:
            case communication::messages::types::EntityId::RIGHT_TROLL:
            case communication::messages::types::EntityId::RIGHT_ELF:
            case communication::messages::types::EntityId::RIGHT_NIFFLER:
            case communication::messages::types::EntityId::LEFT_WOMBAT:
            case communication::messages::types::EntityId::RIGHT_WOMBAT:
                return true;
            default:
                return false;
        }
    }

    bool isBall(communication::messages::types::EntityId id) {
        switch (id) {
            case communication::messages::types::EntityId::SNITCH:
            case communication::messages::types::EntityId::BLUDGER1:
            case communication::messages::types::EntityId::BLUDGER2:
            case communication::messages::types::EntityId::QUAFFLE:
                return true;
            default:
                return false;
        }
    }

    bool isPlayer(communication::messages::types::EntityId id) {
        switch (id) {
            case communication::messages::types::EntityId::LEFT_SEEKER:
            case communication::messages::types::EntityId::LEFT_KEEPER:
            case communication::messages::types::EntityId::LEFT_CHASER1:
            case communication::messages::types::EntityId::LEFT_CHASER2:
            case communication::messages::types::EntityId::LEFT_CHASER3:
            case communication::messages::types::EntityId::LEFT_BEATER1:
            case communication::messages::types::EntityId::LEFT_BEATER2:
            case communication::messages::types::EntityId::RIGHT_SEEKER:
            case communication::messages::types::EntityId::RIGHT_KEEPER:
            case communication::messages::types::EntityId::RIGHT_CHASER1:
            case communication::messages::types::EntityId::RIGHT_CHASER2:
            case communication::messages::types::EntityId::RIGHT_CHASER3:
            case communication::messages::types::EntityId::RIGHT_BEATER1:
            case communication::messages::types::EntityId::RIGHT_BEATER2:
                return true;
            default:
                return false;
        }
    }

    auto idToSide(communication::messages::types::EntityId id) -> gameModel::TeamSide {
        using namespace gameModel;
        switch (id) {
            case communication::messages::types::EntityId::LEFT_SEEKER:
            case communication::messages::types::EntityId::LEFT_KEEPER:
            case communication::messages::types::EntityId::LEFT_CHASER1:
            case communication::messages::types::EntityId::LEFT_CHASER2:
            case communication::messages::types::EntityId::LEFT_CHASER3:
            case communication::messages::types::EntityId::LEFT_BEATER1:
            case communication::messages::types::EntityId::LEFT_BEATER2:
            case communication::messages::types::EntityId::LEFT_GOBLIN:
            case communication::messages::types::EntityId::LEFT_TROLL:
            case communication::messages::types::EntityId::LEFT_ELF:
            case communication::messages::types::EntityId::LEFT_NIFFLER:
            case communication::messages::types::EntityId::LEFT_WOMBAT:
                return TeamSide::LEFT;
            case communication::messages::types::EntityId::RIGHT_GOBLIN:
            case communication::messages::types::EntityId::RIGHT_TROLL:
            case communication::messages::types::EntityId::RIGHT_ELF:
            case communication::messages::types::EntityId::RIGHT_NIFFLER:
            case communication::messages::types::EntityId::RIGHT_WOMBAT:
            case communication::messages::types::EntityId::RIGHT_SEEKER:
            case communication::messages::types::EntityId::RIGHT_KEEPER:
            case communication::messages::types::EntityId::RIGHT_CHASER1:
            case communication::messages::types::EntityId::RIGHT_CHASER2:
            case communication::messages::types::EntityId::RIGHT_CHASER3:
            case communication::messages::types::EntityId::RIGHT_BEATER1:
            case communication::messages::types::EntityId::RIGHT_BEATER2:
                return TeamSide::RIGHT;
            default:
                throw std::runtime_error("No player or Fan");
        }
    }

    auto idToFantype(
            communication::messages::types::EntityId id) -> communication::messages::types::FanType {
        using namespace communication::messages::types;
        switch (id) {
            case communication::messages::types::EntityId::LEFT_GOBLIN:
                return FanType::GOBLIN;
            case communication::messages::types::EntityId::LEFT_TROLL:
                return FanType::TROLL;
            case communication::messages::types::EntityId::LEFT_ELF:
                return FanType::ELF;
            case communication::messages::types::EntityId::LEFT_NIFFLER:
                return FanType::NIFFLER;
            case communication::messages::types::EntityId::LEFT_WOMBAT:
                return FanType::WOMBAT;
            case communication::messages::types::EntityId::RIGHT_GOBLIN:
                return FanType::GOBLIN;
            case communication::messages::types::EntityId::RIGHT_TROLL:
                return FanType::TROLL;
            case communication::messages::types::EntityId::RIGHT_ELF:
                return FanType::ELF;
            case communication::messages::types::EntityId::RIGHT_NIFFLER:
                return FanType::NIFFLER;
            case communication::messages::types::EntityId::RIGHT_WOMBAT:
                return FanType::WOMBAT;
            default:
                throw std::runtime_error("No Fan");
        }
    }

    auto fanToInterference(communication::messages::types::FanType fanType)
    -> gameModel::InterferenceType {
        using namespace communication::messages::types;
        switch (fanType) {
            case FanType::GOBLIN:
                return gameModel::InterferenceType::RangedAttack;
            case FanType::TROLL:
                return gameModel::InterferenceType::Impulse;
            case FanType::ELF:
                return gameModel::InterferenceType::Teleport;
            case FanType::NIFFLER:
                return gameModel::InterferenceType::SnitchPush;
            case FanType::WOMBAT:
                return gameModel::InterferenceType::BlockCell;
            default:
                throw std::runtime_error("Fatal error! Enum out of range");
        }
    }

    auto interferenceToFan(gameModel::InterferenceType type)
    -> communication::messages::types::FanType {
        using namespace communication::messages::types;
        switch (type) {
            case gameModel::InterferenceType::RangedAttack:
                return FanType::GOBLIN;
            case gameModel::InterferenceType::Teleport:
                return FanType::ELF;
            case gameModel::InterferenceType::Impulse:
                return FanType::TROLL;
            case gameModel::InterferenceType::SnitchPush:
                return FanType::NIFFLER;
            case gameModel::InterferenceType::BlockCell:
                return FanType::WOMBAT;
            default:
                throw std::runtime_error("Fatal error! Enum out of range");
        }
    }

}
