#pragma once

#include "SFML/Network.hpp"

#include "Utils/Logger.h"
#include "Utils/vec2.h"
#include "Utils/FrameRate.h"

#include "Networking/Packets.h"
#include "Networking/SocketServer.h"

#include "GameObjects/PlayerInfo.h"

namespace Luntik {
    class Server {
    public:
        Server(short port, sf::IpAddress address)
            : m_Port(port), m_Address(address), m_SocketSever(m_Address, m_Port)
        {
            m_SocketSever.onClientConnected(std::bind(&Server::handleNewConnection, this, std::placeholders::_1, std::placeholders::_2));
            m_SocketSever.onClientDisconnected(std::bind(&Server::handleDisconnection, this, std::placeholders::_1));

            m_SocketSever.setPacketReceiver(Network::Packets::C2S_POSITION_PACKET, std::bind(&Server::handleC2SPositionPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            m_SocketSever.setPacketReceiver(Network::Packets::C2S_NAME_PACKET, std::bind(&Server::handleC2SNamePacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
            m_SocketSever.setPacketReceiver(Network::Packets::C2S_COLOR_PACKET, std::bind(&Server::handleC2SColorPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));
        }

        ~Server() {
            
        }

        void tick(float deltaTime) {
            m_SocketSever.tick();
            checkTempPlayers();
            sendPositionToClients();
        }

        void start() {
            m_SocketSever.start();
        }

        void runAsynchronously() {
            {
                std::lock_guard lock(m_RunServerMutex);
                if (m_RunServer) {
                    LOGGER.log("Tried to run start server asynchronously, but the server is already running");
                    return;
                }
                m_RunServer = true;
            }

            m_ServerThread = std::thread(std::bind(&Server::serverThread, this));
            LOGGER.log("Server started");
        }

        void stop() {
            {
                std::lock_guard<std::mutex> lock(m_RunServerMutex);
                m_RunServer = false;
            }
            if (m_ServerThread.joinable()) {
                LOGGER.log("Waiting for server thread");
                m_ServerThread.join();
            }

            m_SocketSever.stop();
        }

    private:
        void serverThread() {
            Utils::frame_rater<Settings::SEND_POS_RATE> fps;

            bool stopServer = false;

            while (true) {
                {
                    std::lock_guard<std::mutex> lock(m_RunServerMutex);
                    if (!m_RunServer) {
                        stopServer = true;
                    }
                }
                
                if (stopServer) {
                    break;
                }

                // server stuff
                tick(1/Settings::SEND_POS_RATE);
                fps.sleep();
            }
        }

        void sendPositionToClients() {
            for (auto& [id, socket] : m_SocketSever.getClients()) {
                if (m_Players.find(id) == m_Players.end()) continue;
                sf::Packet packet = Network::Packets::createS2CPositionPacket({ id, m_Players.at(id).pos, m_Players.at(id).acc });
                m_SocketSever.broadcastPacketWithout(packet, id);
            }
        }

        void checkTempPlayers() {
            std::vector<Network::ID> removeIds;

            for (auto& [id, tempPlayerInfo] : m_TempPlayers) {
                if (tempPlayerInfo.isReady()) {
                    removeIds.push_back(id);

                    m_Players[id] = tempPlayerInfo;

                    sf::Packet packet = Network::Packets::createS2CPlayerConnectedPacket({ id, m_Players.at(id).name, m_Players.at(id).pos, m_Players.at(id).color });
                    m_SocketSever.broadcastPacketWithout(packet, id);
                }
            }

            for (Network::ID id : removeIds) {
                m_TempPlayers.erase(id);
            }
        }

        void handleNewConnection(Network::ID senderId, sf::TcpSocket* senderSocket) {
            LOGGER.log("New connection established: " + senderSocket->getRemoteAddress().toString() + ":" + std::to_string(senderSocket->getRemotePort()));
            m_TempPlayers[senderId];

            for (auto& [clientId, clientInfo] : m_Players) {
                if (clientId == senderId) continue;
                sf::Packet packet = Network::Packets::createS2CPlayerConnectedPacket({ clientId, clientInfo.name, clientInfo.pos, clientInfo.color });
                senderSocket->send(packet);
            }
        }

        void handleDisconnection(Network::ID senderId) {
            LOGGER.log("Client disconnected: " + std::to_string(senderId));

            // TELL CLIENTS SOMEONE DISCONNECTED
            if (m_Players.erase(senderId)) {
                sf::Packet packet;
                packet = Network::Packets::createS2CPlayerDisconnectedPacket({ senderId });
                m_SocketSever.broadcastPacket(packet);
            }
        }
        
        void handleC2SPositionPacket(Network::ID senderId, sf::TcpSocket* senderSocket, sf::Packet packet) {
            try {
                Network::Packets::C2S_PositionPacketInfo packetInfo = Network::Packets::readC2SPositionPacket(packet);
                if (m_Players.find(senderId) != m_Players.end()) {
                    m_Players.at(senderId).pos = packetInfo.pos;
                    m_Players.at(senderId).acc = packetInfo.acc;
                }
                // LOGGER.log("got pos: " + (std::string)(pos));
            } catch (const std::runtime_error& e) {
                std::cout << "Error while reading packet of type: C2S_POSITION_PACKET\n";
                std::cout << "Sender ID: " << senderId << "\n";
                std::cout << "Error: " << e.what() << std::endl;
            }
        }

        void handleC2SNamePacket(Network::ID senderId, sf::TcpSocket* senderSocket, sf::Packet packet) {
            try {
                Network::Packets::C2S_NamePacketInfo packetInfo = Network::Packets::readC2SNamePacket(packet);
                if (m_Players.find(senderId) != m_Players.end()) {
                    m_Players.at(senderId).name = packetInfo.name;

                    sf::Packet packet = Network::Packets::createS2CNamePacket({ packetInfo.name, senderId });
                    m_SocketSever.broadcastPacketWithout(packet, senderId);
                } else if (m_TempPlayers.find(senderId) != m_TempPlayers.end()) {
                    m_TempPlayers.at(senderId).name = packetInfo.name;
                }
            } catch (const std::runtime_error& e) {
                std::cout << "Error while reading packet of type: C2S_POSITION_PACKET\n";
                std::cout << "Sender ID: " << senderId << "\n";
                std::cout << "Error: " << e.what() << std::endl;
            }
        }

        void handleC2SColorPacket(Network::ID senderId, sf::TcpSocket* senderSocket, sf::Packet packet) {
            try {
                Network::Packets::C2S_ColorPacketInfo packetInfo = Network::Packets::readC2SColorPacket(packet);
                LOGGER.log("Got a color packet from: " + std::to_string(senderId) + ", color is: " + std::to_string(packetInfo.color.x) + " " + std::to_string(packetInfo.color.y) + " " + std::to_string(packetInfo.color.z));
                if (m_Players.find(senderId) != m_Players.end()) {
                    m_Players.at(senderId).color = packetInfo.color;

                    sf::Packet packet = Network::Packets::createS2CColorPacket({ senderId, packetInfo.color });
                    m_SocketSever.broadcastPacketWithout(packet, senderId);
                } else if (m_TempPlayers.find(senderId) != m_TempPlayers.end()) {
                    m_TempPlayers.at(senderId).color = packetInfo.color;
                }
            } catch (const std::runtime_error& e) {
                std::cout << "Error while reading packet of type: C2S_COLOR_PACKET\n";
                std::cout << "Sender ID: " << senderId << "\n";
                std::cout << "Error: " << e.what() << std::endl;
            }
        }

        Utils::Logger LOGGER{"Luntik::Server"};

        // SERVER VARS
        std::unordered_map<Network::ID, GameObjects::PlayerInfo> m_TempPlayers;
        std::unordered_map<Network::ID, GameObjects::PlayerInfo> m_Players;
        
        // NETWORKING
        short m_Port;
        sf::IpAddress m_Address;

        Network::SocketServer m_SocketSever;

        std::thread m_ServerThread;
        std::mutex m_RunServerMutex;
        bool m_RunServer = false;
    };
}