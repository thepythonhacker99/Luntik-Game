#pragma once

#include <memory>

#include "SFML/Network.hpp"

#include "GameObjects/ClientPlayerController.h"
#include "GameObjects/NetworkPlayerController.h"
#include "GameObjects/Player.h"

#include "Renderer/Screens.h"

#include "Utils/Logger.h"
#include "Utils/FrameRate.h"

#include "Networking/SocketClient.h"
#include "Networking/Packets.h"

namespace Luntik {
    enum ConnectionStatus {
        CONNECTED,
        NOT_CONNECTED,
        DISCONNECTED,
        FAILED_TO_CONNECT
    };

    struct OtherPlayer {
        GameObjects::Player player;
        GameObjects::NetworkPlayerController controller;
        uint32_t id;

        OtherPlayer(Utils::vec2 initialPos, uint32_t id) : id(id), controller(nullptr, id) {
            player.setPos(initialPos);
        }
    };

    class Client {
    public:
        Client(short port, sf::IpAddress address) :
            m_Port(port), m_Address(address), m_Player(), m_ClientPlayerController(&m_Player), m_Client(m_Address, m_Port)
        {
            m_Client.setOnDisconnectedFromServerCallback(std::bind(&Client::handleDisconnectedFromServer, this));

            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_PLAYER_CONNECTED_PACKET, std::bind(&Client::handlePlayerConnectedPacket, this, std::placeholders::_1));
            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_PLAYER_DISCONNECTED_PACKET, std::bind(&Client::handlePlayerDisconnectedPacket, this, std::placeholders::_1));
            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_POSITION_PACKET, std::bind(&Client::handlePlayerPositionPacket, this, std::placeholders::_1));

            try {
                m_Client.start();
                m_ConnectionStatus = CONNECTED;
            } catch(const std::exception& e) {
                LOGGER.log(std::string("Error while connecting: ") + e.what());
                m_ConnectionStatus = FAILED_TO_CONNECT;
            }
        }

        ~Client() {
            
        }

        void tick(float deltaTime) {
            m_Client.tick();

            m_Player.tick(deltaTime);
            m_ClientPlayerController.tick(deltaTime);

            for (auto& [id, otherPlayer] : m_OtherPlayers) {
                otherPlayer.controller.tick(deltaTime);
            }

            for (auto& [id, sprite] : s_MainGameScreen->otherPlayers) {
                if (m_OtherPlayers.find(id) == m_OtherPlayers.end()) continue;
                sprite->getImage()->getTransform().setPos(m_OtherPlayers.at(id).player.getPos());
            }

            if (m_SendPositionTimer.should_run_code(deltaTime)) {
                sf::Packet posPacket = Network::Packets::createC2SPositionPacket({ m_Player.getPos(), int(m_Player.getAcc().x) });
                m_Client.getSocket()->send(posPacket);
            }
        }

        void stop() {
            LOGGER.log("Stopping client");
            m_Client.stop();
            m_ConnectionStatus = NOT_CONNECTED;
            LOGGER.log("Client stopped");
        }

        ConnectionStatus getConnectionStatus() { return m_ConnectionStatus; }

    private:
        void handleDisconnectedFromServer() {
            LOGGER.log("Got disconnected from server");
            m_ConnectionStatus = DISCONNECTED;
        }

        void handlePlayerConnectedPacket(sf::Packet packet) {
            LOGGER.log("New player connected");
            try {
                Network::Packets::S2C_PlayerConnectedPacketInfo packetInfo = Network::Packets::readS2CPlayerConnectedPacket(packet);
                LOGGER.log("New player ID: " + std::to_string(packetInfo.id));
                LOGGER.log("New player name: " + packetInfo.name);
                LOGGER.log("New player pos: " + (std::string)(packetInfo.pos));

                if (m_OtherPlayers.count(packetInfo.id) != 0) {
                    LOGGER.log("Player already exists in client!"); 
                    return;
                }

                m_OtherPlayers.emplace(packetInfo.id, OtherPlayer(packetInfo.pos, packetInfo.id));
                m_OtherPlayers.at(packetInfo.id).controller.setPlayer(&m_OtherPlayers.at(packetInfo.id).player);
                m_OtherPlayers.at(packetInfo.id).controller.setGoal(packetInfo.pos);
                // m_OtherPlayers.at(packetInfo.id).setName(packetInfo.name);

                s_MainGameScreen->addOtherPlayer(packetInfo.id);
            } catch (const std::exception& e) {
                LOGGER.log(std::string("Error while handling a new player: ") + e.what());
            }
        }

        void handlePlayerDisconnectedPacket(sf::Packet packet) {
            LOGGER.log("A player disconnected");
            try {
                Network::Packets::S2C_PlayerDisconnectedPacketInfo packetInfo = Network::Packets::readS2CPlayerDisconnectedPacket(packet);
                LOGGER.log("Disconnected player ID: " + std::to_string(packetInfo.id));

                if (m_OtherPlayers.count(packetInfo.id) == 0) {
                    LOGGER.log("Player does not exist in client!"); 
                    return;
                }

                m_OtherPlayers.erase(packetInfo.id);
                s_MainGameScreen->removeOtherPlayer(packetInfo.id);
            } catch (const std::exception& e) {
                LOGGER.log(std::string("Error while player disconnection: ") + e.what());
            }
        }

        void handlePlayerPositionPacket(sf::Packet packet) {
            try {
                Network::Packets::S2C_PositionPacketInfo packetInfo = Network::Packets::readS2CPositionPacket(packet);

                if (m_OtherPlayers.count(packetInfo.id) == 0) {
                    LOGGER.log("Player with id " + std::to_string(packetInfo.id) + " does not exist in client! (handlePlayerPositionPacket)");
                    return;
                }

                m_OtherPlayers.at(packetInfo.id).controller.setGoal(packetInfo.pos);
                m_OtherPlayers.at(packetInfo.id).player.setAcc({ float(packetInfo.moveDir), 0 });
            } catch (const std::exception& e) {
                LOGGER.log(std::string("Error while handling a new position packet: ") + e.what());
            }
        }

        Utils::Logger LOGGER{"Luntik::Client"};

        std::unordered_map<Network::ID, OtherPlayer> m_OtherPlayers;

        GameObjects::Player m_Player;
        GameObjects::ClientPlayerController m_ClientPlayerController;

        Utils::no_sleep_timer<20> m_SendPositionTimer;

        // NETWORKING
        short m_Port;
        sf::IpAddress m_Address;

        Network::SocketClient m_Client;
        ConnectionStatus m_ConnectionStatus = NOT_CONNECTED;
    };
}