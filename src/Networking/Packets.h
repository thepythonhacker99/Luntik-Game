#pragma once

#include "SFML/Network.hpp"

#include "../Utils/vec2.h"

namespace Luntik::Network {
    using ID = uint32_t;
}

namespace Luntik::Network::Packets {
    enum PacketType {
        S2C_PLAYER_CONNECTED_PACKET,
        S2C_PLAYER_DISCONNECTED_PACKET,

        C2S_POSITION_PACKET,
        S2C_POSITION_PACKET
    };
}

sf::Packet& operator<<(sf::Packet& packet, const Luntik::Network::Packets::PacketType& packetType)
{
    packet << static_cast<std::underlying_type_t<Luntik::Network::Packets::PacketType>>(packetType);
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Luntik::Network::Packets::PacketType& packetType)
{
    std::underlying_type_t<Luntik::Network::Packets::PacketType> underlying;
    packet >> underlying;
    packetType = static_cast<Luntik::Network::Packets::PacketType>(underlying);
    return packet;
}


sf::Packet& operator<<(sf::Packet& packet, const Luntik::Utils::vec2& vec)
{
    packet << vec.x << vec.y;
    return packet;
}

sf::Packet& operator>>(sf::Packet& packet, Luntik::Utils::vec2& vec)
{
    packet >> vec.x >> vec.y;
    return packet;
}

namespace Luntik::Network::Packets {
    template<typename T>
    T read(sf::Packet& packet) {
        if (!packet.endOfPacket()) {
            T val;
            if (packet >> val) {
                return val;
            } else {
                std::cout << "Could not read packet - reading failed\n";
                throw std::runtime_error("Could not read packet - reading failed");
            }
        }

        std::cout << "Could not read packet - packet does not have more bytes\n";
        throw std::runtime_error("Could not read packet - packet does not have more bytes");
    }


    // C2S_POSITION_PACKET
    struct C2S_PositionPacketInfo {
        Utils::vec2 pos;

        // -1 - LEFT
        //  1 - RIGHT
        //  0 - NOT MOVING
        int moveDir;
    };

    sf::Packet createC2SPositionPacket(const C2S_PositionPacketInfo& info) {
        sf::Packet packet;
        packet << C2S_POSITION_PACKET;
        packet << info.pos;
        packet << info.moveDir;
        return packet;
    }

    C2S_PositionPacketInfo readC2SPositionPacket(sf::Packet& packet) {
        C2S_PositionPacketInfo info;
        info.pos = read<Utils::vec2>(packet);
        info.moveDir = read<int>(packet);
        return info;
    }

    // S2C_POSITION_PACKET
    struct S2C_PositionPacketInfo {
        ID id;
        Utils::vec2 pos;

        // -1 - LEFT
        //  1 - RIGHT
        //  0 - NOT MOVING
        int moveDir;
    };

    sf::Packet createS2CPositionPacket(const S2C_PositionPacketInfo& info) {
        sf::Packet packet;
        packet << S2C_POSITION_PACKET;
        packet << info.id;
        packet << info.pos;
        packet << info.moveDir;
        return packet;
    }

    S2C_PositionPacketInfo readS2CPositionPacket(sf::Packet& packet) {
        S2C_PositionPacketInfo info;
        info.id = read<ID>(packet);
        info.pos = read<Utils::vec2>(packet);
        info.moveDir = read<int>(packet);
        return info;
    }

    // S2C_PLAYER_CONNECTED_PACKET
    struct S2C_PlayerConnectedPacketInfo {
        ID id;
        std::string name;
        Utils::vec2 pos;
    };

    sf::Packet createS2CPlayerConnectedPacket(const S2C_PlayerConnectedPacketInfo& info) {
        sf::Packet packet;
        packet << S2C_PLAYER_CONNECTED_PACKET;
        packet << info.id;
        packet << info.name;
        packet << info.pos;
        return packet;
    }

    S2C_PlayerConnectedPacketInfo readS2CPlayerConnectedPacket(sf::Packet& packet) {
        S2C_PlayerConnectedPacketInfo info;
        info.id = read<ID>(packet);
        info.name = read<std::string>(packet);
        info.pos = read<Utils::vec2>(packet);
        return info;
    }

    // S2C_PLAYER_DISCONNECTED_PACKET
    struct S2C_PlayerDisconnectedPacketInfo {
        ID id;
    };

    sf::Packet createS2CPlayerDisconnectedPacket(const S2C_PlayerDisconnectedPacketInfo& info) {
        sf::Packet packet;
        packet << S2C_PLAYER_DISCONNECTED_PACKET;
        packet << info.id;
        return packet;
    }

    S2C_PlayerDisconnectedPacketInfo readS2CPlayerDisconnectedPacket(sf::Packet& packet) {
        S2C_PlayerDisconnectedPacketInfo info;
        info.id = read<ID>(packet);
        return info;
    }
}