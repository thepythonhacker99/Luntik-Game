#pragma once

#include <memory>

#include "random.hpp"

#include "SFML/Network.hpp"

#include "GameObjects/ClientPlayerController.h"
#include "GameObjects/NetworkPlayerController.h"
#include "GameObjects/PlayerInfo.h"
#include "GameObjects/ChunkInfo.h"
#include "GameObjects/ChunkManager.h"

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
        GameObjects::PlayerInfo playerInfo;
        GameObjects::NetworkPlayerController controller;
        uint32_t id;

        OtherPlayer(Utils::vec2 initialPos, uint32_t id) : id(id), controller(nullptr, id) {
            playerInfo.pos = initialPos;
        }
    };

    class Client {
    public:
        Client(short port, sf::IpAddress address) :
            m_Port(port), m_Address(address), m_PlayerInfo(), m_ClientPlayerController(&m_PlayerInfo), m_Client(m_Address, m_Port)
        {
            m_Client.setOnDisconnectedFromServerCallback(std::bind(&Client::handleDisconnectedFromServer, this));

            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_PLAYER_CONNECTED_PACKET, std::bind(&Client::handlePlayerConnectedPacket, this, std::placeholders::_1));
            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_PLAYER_DISCONNECTED_PACKET, std::bind(&Client::handlePlayerDisconnectedPacket, this, std::placeholders::_1));
            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_POSITION_PACKET, std::bind(&Client::handlePlayerPositionPacket, this, std::placeholders::_1));
            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_NAME_PACKET, std::bind(&Client::handlePlayerNamePacket, this, std::placeholders::_1));
            m_Client.setPacketReceiver(Network::Packets::PacketType::S2C_COLOR_PACKET, std::bind(&Client::handlePlayerColorPacket, this, std::placeholders::_1));

            try {
                m_Client.start();
                m_ConnectionStatus = CONNECTED;
            } catch(const std::exception& e) {
                LOGGER.log(std::string("Error while connecting: ") + e.what());
                m_ConnectionStatus = FAILED_TO_CONNECT;
            }

            m_PlayerInfo.name = s_IntroScreen->textBox->getText();
            s_MainGameScreen->renderedPlayer->setPlayer(&m_PlayerInfo);

            m_PlayerInfo.color.x = Random::get(0.f, 1.f);
            m_PlayerInfo.color.y = Random::get(0.f, 1.f);
            m_PlayerInfo.color.z = Random::get(0.f, 1.f);

            sf::Packet namePacket = Network::Packets::createC2SNamePacket({ m_PlayerInfo.name });
            m_Client.getSocket()->send(namePacket);

            sf::Packet colorPacket = Network::Packets::createC2SColorPacket({ m_PlayerInfo.color });
            m_Client.getSocket()->send(colorPacket);
        }

        ~Client() {
            
        }

        void tick(float deltaTime) {
            if (!getPaused()) { // when game is not paused
                if (Utils::KeySystem::s_KeySystem->keyState(sf::Keyboard::Key::Escape) == Utils::KeySystem::JUST_PRESSED) {
                    s_Renderer->setScreen(s_PauseScreen.get());
                    setPaused(true);
                }
                
                m_ClientPlayerController.tick(deltaTime);
                s_MainGameScreen->chunkManager->tick(deltaTime);
            } else { // when game is paused
                m_PlayerInfo.acc = { 0, 0 };
                m_ClientPlayerController.setVel({ 0, 0 });
            }

            // regardless if the game is paused or not
            m_Client.tick();

            for (auto& [id, otherPlayer] : m_OtherPlayers) {
                otherPlayer.controller.tick(deltaTime);
            }

            if (m_SendPositionTimer.should_run_code(deltaTime)) {
                sf::Packet posPacket = Network::Packets::createC2SPositionPacket({ m_PlayerInfo.pos, m_PlayerInfo.acc });
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

        bool getPaused() const { return m_Paused; }
        void setPaused(bool paused) { m_Paused = paused; }

    private:
        void createOtherPlayer(Network::ID id, Utils::vec2 pos, const std::string& name, sf::Vector3f color) {
            m_OtherPlayers.emplace(id, OtherPlayer(pos, id));
            m_OtherPlayers.at(id).controller.setPlayer(&m_OtherPlayers.at(id).playerInfo, id);
            m_OtherPlayers.at(id).controller.setGoal(pos);
            m_OtherPlayers.at(id).playerInfo.name = name;
            m_OtherPlayers.at(id).playerInfo.color = color;

            s_MainGameScreen->addOtherPlayer(id, &m_OtherPlayers.at(id).playerInfo);
        }

        void handleDisconnectedFromServer() {
            LOGGER.log("Got disconnected from server");
            m_ConnectionStatus = DISCONNECTED;
        }

        void handlePlayerConnectedPacket(sf::Packet packet) {
            LOGGER.log("New player connected");
            try {
                Network::Packets::S2C_PlayerConnectedPacketInfo packetInfo = Network::Packets::readS2CPlayerConnectedPacket(packet);
                if (m_OtherPlayers.find(packetInfo.id) != m_OtherPlayers.end()) {
                    LOGGER.log("Player already exists in client!");
                    m_OtherPlayers.at(packetInfo.id).playerInfo.pos = packetInfo.pos;
                    return;
                }

                LOGGER.log("New player ID: " + std::to_string(packetInfo.id));
                LOGGER.log("New player name: " + packetInfo.name);
                LOGGER.log("New player pos: " + (std::string)(packetInfo.pos));
                LOGGER.log("New player color: " + std::to_string(packetInfo.color.x) + " " + std::to_string(packetInfo.color.y) + " " + std::to_string(packetInfo.color.z));

                createOtherPlayer(packetInfo.id, packetInfo.pos, packetInfo.name, packetInfo.color);
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
                m_OtherPlayers.at(packetInfo.id).playerInfo.acc = packetInfo.acc;
            } catch (const std::exception& e) {
                LOGGER.log(std::string("Error while handling a new position packet: ") + e.what());
            }
        }

        void handlePlayerNamePacket(sf::Packet packet) {
            try {
                Network::Packets::S2C_NamePacketInfo packetInfo = Network::Packets::readS2CNamePacket(packet);

                if (m_OtherPlayers.find(packetInfo.id) == m_OtherPlayers.end()) {
                    // createOtherPlayer(packetInfo.id, Utils::vec2(0, 0), packetInfo.name);
                    LOGGER.log("Player with id " + std::to_string(packetInfo.id) + " does not exist in client! (handlePlayerNamePacket)");
                    return;
                }
                m_OtherPlayers.at(packetInfo.id).playerInfo.name = packetInfo.name;
                s_MainGameScreen->otherPlayers.at(packetInfo.id)->updateNameText();
            } catch (const std::exception& e) {
                LOGGER.log(std::string("Error while handling a new position packet: ") + e.what());
            }
        }

        void handlePlayerColorPacket(sf::Packet packet) {
            try {
                Network::Packets::S2C_ColorPacketInfo packetInfo = Network::Packets::readS2CColorPacket(packet);
                LOGGER.log("Got a color packet");
                LOGGER.log("Id: " + std::to_string(packetInfo.id));
                LOGGER.log("Color: " + std::to_string(packetInfo.color.x) + " " + std::to_string(packetInfo.color.y) + " " + std::to_string(packetInfo.color.z));

                if (m_OtherPlayers.find(packetInfo.id) == m_OtherPlayers.end()) {
                    // createOtherPlayer(packetInfo.id, Utils::vec2(0, 0), packetInfo.name);
                    LOGGER.log("Player with id " + std::to_string(packetInfo.id) + " does not exist in client! (handlePlayerColorPacket)");
                    return;
                }

                m_OtherPlayers.at(packetInfo.id).playerInfo.color = packetInfo.color;
            } catch (const std::exception& e) {
                LOGGER.log(std::string("Error while handling a color packet: ") + e.what());
            }
        }

        Utils::Logger LOGGER{"Luntik::Client"};

        bool m_Paused = false;

        std::unordered_map<Network::ID, OtherPlayer> m_OtherPlayers;

        GameObjects::PlayerInfo m_PlayerInfo;
        GameObjects::ClientPlayerController m_ClientPlayerController;

        Utils::no_sleep_timer<20> m_SendPositionTimer;

        // NETWORKING
        short m_Port;
        sf::IpAddress m_Address;

        Network::SocketClient m_Client;
        ConnectionStatus m_ConnectionStatus = NOT_CONNECTED;
    };
}