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

#include "Client.h"
#include "Server.h"

#include "Utils/KeySystem.h"
#include "Utils/Logger.h"
#include "Utils/FrameRate.h"

#include "SFML/Network.hpp"

namespace Luntik {
    class Game {
    public:
        Game() {
            Renderer::Textures::loadTextures();
            Renderer::Animations::loadAnimations();
            Renderer::Fonts::loadFonts();

            m_Renderer = std::make_unique<Renderer::Renderer>();
            m_Renderer->getWindow()->getSFMLWindow()->setVerticalSyncEnabled(true);
            
            Utils::KeySystem::initializeKeySystem(m_Renderer->getWindow());

            m_ScreenMainGame = std::make_unique<Renderer::Screens::MainGameScreen>();
            m_ScreenIntro = std::make_unique<Renderer::Screens::IntroScreen>();
            m_ScreenDisconnected = std::make_unique<Renderer::Screens::DisconnectedScreen>();

            m_Renderer->setScreen(m_ScreenIntro.get());
        }

        ~Game() {
            
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
                    LOGGER.log("FPS: " + std::to_string(frames));
                    time = 0;
                    frames = 0;
                }

                Utils::KeySystem::s_KeySystem->update();

                switch (m_Renderer->getScreen()->ID) {
                    case Renderer::Screens::MAIN_GAME_SCREEN:
                        if (m_Client.get()) {
                            if (m_Client->getConnectionStatus() != CONNECTED) {
                                std::string disconnectMessage = "";
                                switch (m_Client->getConnectionStatus()) {
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
                                m_ScreenDisconnected->disconnectedText->setText(disconnectMessage);
                                m_Renderer->setScreen(m_ScreenDisconnected.get());

                                m_ScreenMainGame.reset(new Renderer::Screens::MainGameScreen());
                                m_Client.reset();
                                {
                                    std::lock_guard<std::mutex> lock(m_RunServerMutex);
                                    m_RunServer = false;
                                }
                            }
                            else m_Client->tick(deltaTime);
                        }
                        break;

                    case Renderer::Screens::INTRO_SCREEN:
                        if (m_ScreenIntro->joinButton->isPressed(m_Renderer->getWindow())) {
                            initClient();
                            m_Renderer->setScreen(m_ScreenMainGame.get());
                        } else if (m_ScreenIntro->hostButton->isPressed(m_Renderer->getWindow())) {
                            runServer();
                            initClient();                            

                            m_Renderer->setScreen(m_ScreenMainGame.get());
                        }
                        break;

                    case Renderer::Screens::DISCONNECTED_SCREEN:
                        if (m_ScreenDisconnected->backButton->isPressed(m_Renderer->getWindow())) {
                            m_Renderer->setScreen(m_ScreenIntro.get());
                        }
                        break;

                    default:
                        returnCode = 1;
                        run = false;
                        break;
                }

                if (run) {
                    if (m_Renderer->render(deltaTime)) {
                        run = false;
                    }
                }
            }
            m_Client->stop();

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
            if (m_Client.get() == nullptr) {
                m_Client = std::make_unique<Client>(m_ScreenMainGame.get(), 13553, sf::IpAddress::getLocalAddress());
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

                    while (true) {
                        {
                            std::lock_guard<std::mutex> lock(*runServerMutex);
                            if (!*runServer) {
                                server->stop();
                                break;
                            }
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

        sf::Clock timer;

        std::thread m_ServerThread;

        bool m_RunServer = false;
        std::mutex m_RunServerMutex;

        std::unique_ptr<Renderer::Screens::MainGameScreen> m_ScreenMainGame;
        std::unique_ptr<Renderer::Screens::IntroScreen> m_ScreenIntro;
        std::unique_ptr<Renderer::Screens::DisconnectedScreen> m_ScreenDisconnected;

        std::unique_ptr<Client> m_Client;

        std::unique_ptr<Luntik::Renderer::Renderer> m_Renderer;
    };
}
