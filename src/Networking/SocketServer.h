#pragma once

#include "SFML/Network.hpp"

#include "../Utils/Logger.h"

#include "Packets.h"

#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <unordered_map>

namespace Luntik::Network {
    class SocketServer {
    public:
        using CallbackOnClientConnected = std::function<void(ID, sf::TcpSocket*)>;
        using CallbackOnClientDisconnected = std::function<void(ID)>;
        using CallbackOnPacketReceived = std::function<void(ID, sf::TcpSocket*, sf::Packet)>;

        struct ClientInfo {
            ID id;
            sf::TcpSocket* socket = nullptr;
            std::thread clientThread;
            
            std::shared_ptr<bool> runClientThread;
            std::shared_ptr<std::mutex> runClientMutex;

            ClientInfo() {
                runClientThread = std::make_shared<bool>(true);
                runClientMutex = std::make_shared<std::mutex>();
            }
        };

        struct PacketInfo {
            ID senderID;
            sf::Packet packet;
        };
        
        SocketServer(sf::IpAddress addr, short port) : m_Port(port), m_Address(addr) {
            
        }

        ~SocketServer() {

        }

        void onClientConnected(CallbackOnClientConnected callback) { m_CallbackOnClientConnected = callback; }
        void onClientDisconnected(CallbackOnClientDisconnected callback) { m_CallbackOnClientDisconnected = callback; }

        void setPacketReceiver(Packets::PacketType packetType, CallbackOnPacketReceived callback) { m_CallbacksOnPacketReceived[packetType] = callback; }

        void start() {
            startListen();
        }

        void tick() {
            handleNewConnections();
            handleDisconnections();
            handleReceivedPackets();
        }

        void stop() {
            LOGGER.log("Shutting down SocketServer");
            // MAKE ALL THE THREADS STOP
            {
                std::lock_guard<std::mutex> lock(m_ListenMutex);
                m_Listen = false;
                m_Listener.close();
            }
            for (auto& [id, clientInfo] : m_Clients) {
                std::lock_guard<std::mutex> lock(*clientInfo.runClientMutex);
                clientInfo.runClientThread.reset(new bool{false});
                clientInfo.socket->disconnect();
            }
            /////

            // WAIT FOR ALL THE THREADS
            if (m_ListenerThread.joinable()) {
                LOGGER.log("Waiting for the listener thread");
                m_ListenerThread.join();
            }
            LOGGER.log("Listener thread finished");
            /////

            // DISCONNECT THE SOCKETS AND WAIT FOR THE CLIENT THREADS
            LOGGER.log("Waiting for the client threads");
            for (auto& [id, clientInfo] : m_Clients) {
                if (clientInfo.clientThread.joinable()) {
                    LOGGER.log("Waiting for client " + std::to_string(id));
                    clientInfo.clientThread.join();
                    LOGGER.log("Client " + std::to_string(id) + " has finished");
                }
                delete clientInfo.socket;
            }
            /////

            // CLEANUP
            while (!m_ConnectedClientsQueue.empty()) {
                delete m_ConnectedClientsQueue.front();
            }
            /////

            LOGGER.log("Successfully shut down SocketServer");
        }

        void broadcastPacketWithout(const sf::Packet& packet, ID withoutId=-1) {
            for (auto& [id, clientInfo] : m_Clients) {
                if (id == withoutId) continue;
                sf::Packet p = packet;
                if (clientInfo.socket->send(p) != sf::Socket::Done) {
                    LOGGER.log("Broadcasting packet to client: " + std::to_string(id) + " failed!");
                }
            }
        }

        void broadcastPacket(const sf::Packet& packet) {
            broadcastPacketWithout(packet); // to avoid copying code
        }

        std::unordered_map<ID, ClientInfo>& getClients() { return m_Clients; }

    private:
        void handleNewConnections() {
            std::lock_guard<std::mutex> lock(m_CallbackConnectedClientsQueueMutex);
            
            while (!m_ConnectedClientsQueue.empty()) {
                sf::TcpSocket* client = m_ConnectedClientsQueue.front();

                ClientInfo clientInfo;
                clientInfo.id = ++m_CurrentClientId;
                clientInfo.socket = client;
                m_Clients[clientInfo.id] = std::move(clientInfo);

                m_Clients.at(clientInfo.id).clientThread = std::thread(
                    std::bind(&SocketServer::clientThread, this, std::placeholders::_1),
                    &m_Clients.at(clientInfo.id)
                );

                LOGGER.log("Calling synced OnClientConnected");
                m_CallbackOnClientConnected(clientInfo.id, client);

                m_ConnectedClientsQueue.pop();
            }
        }

        void handleDisconnections() {
            std::lock_guard<std::mutex> lock(m_CallbackDisconnectedClientsQueueMutex);

            while (!m_DisconnectedClientsQueue.empty()) {
                ID id = m_DisconnectedClientsQueue.front();
                LOGGER.log("Removing client: " + std::to_string(id));

                m_Clients.at(id).socket->disconnect();
                delete m_Clients.at(id).socket;

                m_Clients.at(id).clientThread.join();
                m_Clients.erase(id);

                LOGGER.log("Calling synced OnClientDisconnected");
                m_CallbackOnClientDisconnected(id);

                m_DisconnectedClientsQueue.pop();
            }
        }

        void handleReceivedPackets() {
            
            std::lock_guard<std::mutex> lock(m_ReceivedPacketsQueueMutex);

            while (!m_ReceivedPacketsQueue.empty()) {
                PacketInfo packetInfo = m_ReceivedPacketsQueue.front();

                if (packetInfo.packet.endOfPacket()) {
                    LOGGER.log("Received an empty packet");
                    m_ReceivedPacketsQueue.pop();
                    continue;
                }

                Packets::PacketType packetType;
                if (packetInfo.packet >> packetType) {
                    if (m_CallbacksOnPacketReceived.count(packetType) > 0) {
                        m_CallbacksOnPacketReceived.at(packetType)(packetInfo.senderID, m_Clients.at(packetInfo.senderID).socket, packetInfo.packet);
                    }
                } else {
                    LOGGER.log("Error while retrieving packet type");
                }

                m_ReceivedPacketsQueue.pop();
            }
        }

        void clientThread(ClientInfo* client) {
            while (true) {
                {
                    std::lock_guard<std::mutex> lock(*client->runClientMutex);
                    if (!*client->runClientThread) {
                        break;
                    }
                }

                try {
                    sf::Packet packet;
                    sf::Socket::Status receiveStatus = client->socket->receive(packet);

                    if (receiveStatus != sf::Socket::Done) {
                        LOGGER.log("Receive status is not sf::Socket::Done. (Status: " + std::to_string(receiveStatus) + ")");
                        break;
                    }

                    {
                        std::lock_guard<std::mutex> lock(m_ReceivedPacketsQueueMutex);

                        PacketInfo packetInfo;
                        packetInfo.packet = packet;
                        packetInfo.senderID = client->id;

                        m_ReceivedPacketsQueue.push(packetInfo);
                    }
                    
                } catch (const std::exception &e) {
                    this->LOGGER.log(std::string("Error receiving packet: ") + e.what());
                    break;
                }
            }

            this->LOGGER.log("Finished Client " + std::to_string(client->id));
            
            std::lock_guard<std::mutex> lock(this->m_CallbackDisconnectedClientsQueueMutex);
            m_DisconnectedClientsQueue.push(client->id);
            LOGGER.log("Added client " + std::to_string(client->id) + " to disconnect queue");
        }

        void startListen() {
            m_Listen = true;
            m_Listener.listen(m_Port, m_Address);
            LOGGER.log("Listening on port " + std::to_string(m_Port) + ", address " + m_Address.toString());

            m_ListenerThread = std::thread(
                [this]() {
                    while (true) {
                        {
                            std::lock_guard<std::mutex> lock(m_ListenMutex);
                            if (!m_Listen) {
                                break;
                            }
                        }

                        // accept clients
                        sf::TcpSocket* socket = new sf::TcpSocket();
                        sf::Socket::Status status = m_Listener.accept(*socket);

                        if (status == sf::Socket::Done) {
                            {
                                std::lock_guard<std::mutex> lock(m_CallbackConnectedClientsQueueMutex);
                                m_ConnectedClientsQueue.push(socket);
                            }

                            LOGGER.log("Accepted a client");
                        } else {
                            LOGGER.log("Error while accepting client");
                            delete socket;
                        }
                    }

                    LOGGER.log("Finished listening");
                }
            );
        }

        Utils::Logger LOGGER{"Luntik::SocketServer"};

        // ADDRESS
        short m_Port;
        sf::IpAddress m_Address;

        // VARIABLES
        ID m_CurrentClientId = 0;
        std::unordered_map<ID, ClientInfo> m_Clients;

        // CALLBACKS
        std::mutex m_CallbackConnectedClientsQueueMutex;
        std::queue<sf::TcpSocket*> m_ConnectedClientsQueue;
        CallbackOnClientConnected m_CallbackOnClientConnected = {};

        std::mutex m_CallbackDisconnectedClientsQueueMutex;
        std::queue<ID> m_DisconnectedClientsQueue;
        CallbackOnClientDisconnected m_CallbackOnClientDisconnected = {};

        std::mutex m_ReceivedPacketsQueueMutex;
        std::queue<PacketInfo> m_ReceivedPacketsQueue;
        std::unordered_map<Packets::PacketType, CallbackOnPacketReceived> m_CallbacksOnPacketReceived;

        // LISTENER
        sf::TcpListener m_Listener;

        bool m_Listen = false;
        std::mutex m_ListenMutex;

        std::thread m_ListenerThread;
    };
}