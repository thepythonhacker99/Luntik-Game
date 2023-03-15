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

    class SocketClient {
    public:
        using CallbackOnPacketReceived = std::function<void(sf::Packet)>;
        using CallbackOnDisconnected = std::function<void()>;

        SocketClient(sf::IpAddress addr, short port) : m_Port(port), m_Address(addr) {
            
        }

        ~SocketClient() {

        }

        void setOnDisconnectedFromServerCallback(CallbackOnDisconnected callback) { m_CallbackOnDisconnectedFromServer = callback; }
        void setPacketReceiver(Packets::PacketType packetType, CallbackOnPacketReceived callback) { m_CallbacksOnPacketReceived[packetType] = callback; }

        void start() {
            if (m_Socket.connect(m_Address, m_Port, sf::seconds(3.f)) == sf::Socket::Done) {
                LOGGER.log("Connected to server: " + m_Address.toString() + " " + std::to_string(m_Port));

                m_Listen = true;
                m_ListenThread = std::thread(std::bind(&SocketClient::listenThread, this));
            } else {
                LOGGER.log("Error: Couldn't connect to the server" + m_Address.toString() + ":" + std::to_string(m_Port));
                throw std::runtime_error("Couldn't connect to the server: " + m_Address.toString() + ":" + std::to_string(m_Port));
            }
        }

        void tick() {
            handleReceivedPackets();

            std::lock_guard<std::mutex> lock(m_DisconnectedFromServerMutex);
            if (m_DisconnectedFromServer) {
                m_CallbackOnDisconnectedFromServer();
                m_DisconnectedFromServer = false;
            }
        }

        void stop() {
            LOGGER.log("Stopping socket client");
            // MAKE ALL THE THREADS STOP
            {
                std::lock_guard<std::mutex> lock(m_ListenMutex);
                m_Listen = false;
                m_Socket.disconnect();
            }
            /////

            // WAIT FOR ALL THE THREADS
            if (m_ListenThread.joinable()) {
                LOGGER.log("Waiting for the listener thread");
                m_ListenThread.join();
            }
            LOGGER.log("Listener thread finished");
            /////

            // CLEANUP
            
            /////
            LOGGER.log("Successfully shutdown the socket client");
        }

        sf::TcpSocket* getSocket() {
            return &m_Socket;
        }

    private:
        void handleReceivedPackets() {
            std::lock_guard<std::mutex> lock(m_ReceivedPacketsQueueMutex);

            while (!m_ReceivedPacketsQueue.empty()) {
                sf::Packet packet = m_ReceivedPacketsQueue.front();

                if (packet.endOfPacket()) {
                    LOGGER.log("Received an empty packet");
                    m_ReceivedPacketsQueue.pop();
                    continue;
                }

                Packets::PacketType packetType;
                if (packet >> packetType) {
                    if (m_CallbacksOnPacketReceived.count(packetType) > 0) {
                        m_CallbacksOnPacketReceived.at(packetType)(packet);
                    }
                } else {
                    LOGGER.log("Error while retrieving packet type");
                }

                m_ReceivedPacketsQueue.pop();
            }
        }

        void listenThread() {
            while (true) {
                {
                    std::lock_guard<std::mutex> lock(m_ListenMutex);
                    if (!m_Listen) {
                        break;
                    }
                }

                try {
                    sf::Packet packet;
                    sf::Socket::Status receiveStatus = m_Socket.receive(packet);

                    if (receiveStatus != sf::Socket::Done) {
                        LOGGER.log("Receive status is not sf::Socket::Done. (Status: " + std::to_string(receiveStatus) + ")");
                        break;
                    }

                    {
                        std::lock_guard<std::mutex> lock(m_ReceivedPacketsQueueMutex);
                        m_ReceivedPacketsQueue.push(packet);
                    }
                } catch (const std::exception &e) {
                    LOGGER.log(std::string("Error receiving packet: ") + e.what());
                    break;
                }
            }

            {
                std::lock_guard<std::mutex> lock(m_ListenMutex);
                if (m_Listen == true) {
                    m_Listen = false;

                    std::lock_guard<std::mutex> lock(m_DisconnectedFromServerMutex);
                    m_DisconnectedFromServer = true;
                }
            }

            LOGGER.log("Finished listening");
            m_Socket.disconnect();
        }

        Utils::Logger LOGGER{"Luntik::SocketClient"};

        // ADDRESS
        short m_Port;
        sf::IpAddress m_Address;

        // CALLBACKS
        std::mutex m_ReceivedPacketsQueueMutex;
        std::queue<sf::Packet> m_ReceivedPacketsQueue;
        std::unordered_map<Packets::PacketType, CallbackOnPacketReceived> m_CallbacksOnPacketReceived;

        std::mutex m_DisconnectedFromServerMutex;
        bool m_DisconnectedFromServer = false;
        CallbackOnDisconnected m_CallbackOnDisconnectedFromServer = {};
        // LISTENER
        sf::TcpSocket m_Socket;

        bool m_Listen = false;
        std::mutex m_ListenMutex;

        std::thread m_ListenThread;
    };
}