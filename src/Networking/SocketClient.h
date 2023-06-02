#pragma once

#include "SFML/Network.hpp"

#include "../Utils/Logger.h"

#include "Packets.h"

#include <mutex>
#include <thread>
#include <queue>
#include <functional>
#include <unordered_map>
#include <memory>

namespace Luntik::Network {

    class SocketClient {
    public:
        using CallbackOnPacketReceived = std::function<void(sf::Packet)>;
        using CallbackOnDisconnected = std::function<void()>;

        SocketClient(sf::IpAddress addr, short port) : m_Port(port), m_Address(addr) {
            s_ClientSocket.reset(new sf::TcpSocket());
        }

        ~SocketClient() {
            s_ClientSocket.reset();
        }

        void setOnDisconnectedFromServerCallback(CallbackOnDisconnected callback) { m_CallbackOnDisconnectedFromServer = callback; }
        void setPacketReceiver(Packets::PacketType packetType, CallbackOnPacketReceived callback) { m_CallbacksOnPacketReceived[packetType] = callback; }

        void start() {
            if (s_ClientSocket->connect(m_Address, m_Port, sf::seconds(3.f)) == sf::Socket::Done) {
                LOGGER.log("Connected to server: " + m_Address.toString() + " " + std::to_string(m_Port));
                m_Running = true;
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
            if (!m_Running) return;
            m_Running = false;

            LOGGER.log("Stopping socket client");
            // MAKE ALL THE THREADS STOP
            {
                std::lock_guard<std::mutex> lock(m_ListenMutex);
                m_Listen = false;
                s_ClientSocket->disconnect();
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
                    sf::Socket::Status receiveStatus = s_ClientSocket->receive(packet);

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
            s_ClientSocket->disconnect();
        }

        Utils::Logger LOGGER{"Luntik::SocketClient"};

        // ADDRESS
        short m_Port;
        sf::IpAddress m_Address;

        // VARS
        bool m_Running = false;

        // CALLBACKS
        std::mutex m_ReceivedPacketsQueueMutex;
        std::queue<sf::Packet> m_ReceivedPacketsQueue;
        std::unordered_map<Packets::PacketType, CallbackOnPacketReceived> m_CallbacksOnPacketReceived;

        std::mutex m_DisconnectedFromServerMutex;
        bool m_DisconnectedFromServer = false;
        CallbackOnDisconnected m_CallbackOnDisconnectedFromServer = {};

        // LISTENER
        bool m_Listen = false;
        std::mutex m_ListenMutex;

        std::thread m_ListenThread;
    };
}