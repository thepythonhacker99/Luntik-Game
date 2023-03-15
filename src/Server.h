#pragma once

#include "SFML/Network.hpp"

#include "Utils/Logger.h"
#include "Utils/vec2.h"
#include "Utils/FrameRate.h"

#include "Networking/Packets.h"
#include "Networking/SocketServer.h"

#include "GameObjects/Player.h"

namespace Luntik {
    class Server {
    public:
        Server(short port, sf::IpAddress address)
            : m_Port(port), m_Address(address), m_SocketSever(m_Address, m_Port)
        {
            m_SocketSever.onClientConnected(std::bind(&Server::handleNewConnection, this, std::placeholders::_1, std::placeholders::_2));
            m_SocketSever.onClientDisconnected(std::bind(&Server::handleDisconnection, this, std::placeholders::_1));

            m_SocketSever.setPacketReceiver(Network::Packets::C2S_POSITION_PACKET, std::bind(&Server::handleC2SPositionPacket, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

            m_SocketSever.start();
        }

        void tick(float deltaTime) {
            m_SocketSever.tick();
            sendPositionToClients();
        }

        void stop() {
            m_SocketSever.stop();
        }

    private:
        void sendPositionToClients() {
            for (auto& [id, socket] : m_SocketSever.getClients()) {
                if (m_Players.find(id) == m_Players.end()) continue;
                sf::Packet packet = Network::Packets::createS2CPositionPacket({ id, m_Players.at(id).getPos() });
                m_SocketSever.broadcastPacketWithout(packet, id);
            }
        }

        void handleNewConnection(Network::ID senderId, sf::TcpSocket* senderSocket) {
            LOGGER.log("New connection established: " + senderSocket->getRemoteAddress().toString() + ":" + std::to_string(senderSocket->getRemotePort()));
            m_Players[senderId];

            sf::Packet packet = Network::Packets::createS2CPlayerConnectedPacket({ senderId, "", m_Players[senderId].getPos() });
            m_SocketSever.broadcastPacketWithout(packet, senderId);

            for (auto& [clientId, clientInfo] : m_Players) {
                if (clientId == senderId) continue;
                sf::Packet packet = Network::Packets::createS2CPlayerConnectedPacket({ clientId, "", clientInfo.getPos() }); // TODO: replace "" with clientInfo.getName() after adding names
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
                Utils::vec2 pos = Network::Packets::read<Utils::vec2>(packet);
                if (m_Players.count(senderId) > 0) {
                    m_Players.at(senderId).setPos(pos);
                }
                // LOGGER.log("got pos: " + (std::string)(pos));
            } catch (const std::runtime_error& e) {
                std::cout << "Error while reading packet of type: C2S_POSITION_PACKET\n";
                std::cout << "Sender ID: " << senderId << "\n";
                std::cout << "Error: " << e.what() << std::endl;
            }
        }

        Utils::Logger LOGGER{"Luntik::Server"};

        // SERVER VARS
        std::unordered_map<Network::ID, GameObjects::Player> m_Players;
        
        // NETWORKING
        short m_Port;
        sf::IpAddress m_Address;

        Network::SocketServer m_SocketSever;
    };
}