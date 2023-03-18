#pragma once

#include <memory>
#include <mutex>
#include <thread>

#include "Renderer/Renderer.h"

#include "Renderer/Screen.h"
#include "Renderer/Screens.h"

#include "Renderer/Textures.h"
#include "Renderer/Animations.h"
#include "Renderer/Fonts.h"

#include "GameObjects/ClientPlayerController.h"
#include "GameObjects/Player.h"

#include "GameState.h"
#include "Client.h"
#include "Server.h"

#include "Utils/KeySystem.h"
#include "Utils/Logger.h"
#include "Utils/FrameRate.h"

#include "SFML/Network.hpp"

namespace Luntik {
    class Game {
    public:
        Game(const std::string& ip) {
            if (ip == "0") {
                m_Ip = sf::IpAddress::getLocalAddress();
            } else {
                m_Ip = sf::IpAddress(ip);
            }

            Renderer::Textures::loadTextures();
            Renderer::Animations::loadAnimations();
            Renderer::Fonts::loadFonts();

            s_Renderer = new Renderer::Renderer();
            s_Renderer->getWindow()->getSFMLWindow()->setVerticalSyncEnabled(true);
            
            Utils::KeySystem::initializeKeySystem();

            s_MainGameScreen = new Renderer::Screens::MainGameScreen();
            s_IntroScreen = new Renderer::Screens::IntroScreen();
            s_DisconnectedScreen = new Renderer::Screens::DisconnectedScreen();

            s_Renderer->setScreen(s_IntroScreen);
        }

        ~Game() {
            Renderer::Textures::unloadTextures();
            Renderer::Fonts::unloadFonts();
        }

        int run() {
            int frames = 0;
            float time = 0;

            bool run = true;
            int returnCode = 0;

            while (run) {
                float deltaTime = timer.restart().asSeconds();
                time += deltaTime;
                frames++;

                if (time > 1) {
                    time = 0;
                    frames = 0;
                }

                Utils::KeySystem::s_KeySystem->update();

                switch (s_Renderer->getScreen()->ID) {
                    case Renderer::Screens::MAIN_GAME_SCREEN:
                        if (s_Client) {
                            if (s_Client->getConnectionStatus() != CONNECTED) {
                                std::string disconnectMessage = "";
                                switch (s_Client->getConnectionStatus()) {
                                    case DISCONNECTED:
                                        disconnectMessage = "Disconnected";
                                        break;

                                    case FAILED_TO_CONNECT:
                                        disconnectMessage = "Couldn't connect";
                                        break;

                                    default:
                                        disconnectMessage = "Something went wrong";
                                        break;
                                }
                                s_DisconnectedScreen->disconnectedText->setText(disconnectMessage);
                                s_Renderer->setScreen(s_DisconnectedScreen);

                                delete s_MainGameScreen;
                                s_MainGameScreen = new Renderer::Screens::MainGameScreen();

                                delete s_Client;
                                {
                                    std::lock_guard<std::mutex> lock(m_RunServerMutex);
                                    m_RunServer = false;
                                }
                            }
                            else s_Client->tick(deltaTime);
                        }
                        break;

                    case Renderer::Screens::INTRO_SCREEN:
                        if (s_IntroScreen->joinButton->isPressed()) {
                            initClient();
                            s_Renderer->setScreen(s_MainGameScreen);
                        } else if (s_IntroScreen->hostButton->isPressed()) {
                            runServer();
                            initClient();                            

                            s_Renderer->setScreen(s_MainGameScreen);
                        }
                        break;

                    case Renderer::Screens::DISCONNECTED_SCREEN:
                        if (s_DisconnectedScreen->backButton->isPressed()) {
                            s_Renderer->setScreen(s_IntroScreen);
                        }
                        break;

                    default:
                        returnCode = 1;
                        run = false;
                        break;
                }

                if (run) {
                    if (s_Renderer->render(deltaTime)) {
                        run = false;
                    }
                }
            }

            if (s_Client) {
                s_Client->stop();
            }

            {
                std::lock_guard<std::mutex> lock(m_RunServerMutex);
                m_RunServer = false;
            }

            if (m_ServerThread.joinable()) {
                LOGGER.log("Stopping server");
                m_ServerThread.join();
                LOGGER.log("Server stopped");
            }


            return returnCode;
        }

    private:
        void initClient() {
            if (s_Client == nullptr) {
                s_Client = new Client(s_MainGameScreen, 13553, m_Ip);
                LOGGER.log("Client created");
            } else {
                LOGGER.log("Client already created");
            }
        }

        void runServer() {
            {
                std::lock_guard<std::mutex> lock(m_RunServerMutex);
                if (m_RunServer) {
                    LOGGER.log("Server is already running");
                    return;
                }
            }

            m_RunServer = true;
            m_ServerThread = std::thread(
                [](bool* runServer, std::mutex* runServerMutex) {
                    std::unique_ptr<Server> server = std::make_unique<Server>(13553, sf::IpAddress::getLocalAddress());
                    Utils::frame_rater<Settings::SEND_POS_RATE> fps;

                    bool stopServer = false;

                    while (true) {
                        {
                            std::lock_guard<std::mutex> lock(*runServerMutex);
                            if (!*runServer) {
                                stopServer = true;
                            }
                        }
                        
                        if (stopServer) {
                            server->stop();
                            break;
                        }

                        // server stuff
                        server->tick(1/Settings::SEND_POS_RATE);
                        fps.sleep();
                    }
                },
                &m_RunServer,
                &m_RunServerMutex
            );
            LOGGER.log("Server started");
        }

        Utils::Logger LOGGER{"Luntik"};

        sf::IpAddress m_Ip;

        sf::Clock timer;

        std::thread m_ServerThread;

        bool m_RunServer = false;
        std::mutex m_RunServerMutex;
    };
}
