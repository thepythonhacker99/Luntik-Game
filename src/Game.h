#pragma once

#include <memory>
#include <mutex>
#include <thread>

#include "Renderer/Textures.h"
#include "Renderer/Animations.h"
#include "Renderer/Fonts.h"

#include "GameState.h"
#include "Client.h"
#include "Server.h"

#include "Utils/KeySystem.h"
#include "Utils/Logger.h"
#include "Utils/FrameRate.h"

#include "Renderer/Screens.h"
#include "Renderer/Renderer.h"

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

            s_Renderer = std::make_unique<Renderer::Renderer>();
            s_Renderer->getWindow()->getSFMLWindow()->setVerticalSyncEnabled(true);
            
            Utils::KeySystem::initializeKeySystem();

            s_MainGameScreen = std::make_unique<Renderer::Screens::MainGameScreen>();
            s_IntroScreen = std::make_unique<Renderer::Screens::IntroScreen>();
            s_DisconnectedScreen = std::make_unique<Renderer::Screens::DisconnectedScreen>();

            s_Renderer->setScreen(s_IntroScreen.get());
        }

        ~Game() {
            Renderer::Textures::unloadTextures();
            Renderer::Fonts::unloadFonts();
            GameState::uninitGameState();
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
                                s_Renderer->setScreen(s_DisconnectedScreen.get());

                                s_MainGameScreen.reset(new Renderer::Screens::MainGameScreen());

                                deleteClient();
                                deleteServer();
                            }
                            else s_Client->tick(deltaTime);
                        }
                        break;

                    case Renderer::Screens::INTRO_SCREEN:
                        if (s_IntroScreen->joinButton->isPressed(s_IntroScreen->joinButton->getTransform())) {
                            initClient();
                            s_Renderer->setScreen(s_MainGameScreen.get());
                        } else if (s_IntroScreen->hostButton->isPressed(s_IntroScreen->hostButton->getTransform())) {
                            initServer();
                            s_Server->start();
                            s_Server->runAsynchronously();

                            initClient();
                            if (s_Client->getConnectionStatus() == CONNECTED) {
                                s_Renderer->setScreen(s_MainGameScreen.get());
                            } else {
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
                                deleteClient();
                                deleteServer();

                                s_DisconnectedScreen->disconnectedText->setText(disconnectMessage);
                                s_Renderer->setScreen(s_DisconnectedScreen.get());
                            }
                        }
                        break;

                    case Renderer::Screens::DISCONNECTED_SCREEN:
                        if (s_DisconnectedScreen->backButton->isPressed(s_DisconnectedScreen->backButton->getTransform())) {
                            s_Renderer->setScreen(s_IntroScreen.get());
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

            deleteClient();
            deleteServer();

            return returnCode;
        }

    private:
        void initClient() {
            if (!s_Client) {
                s_Client.reset(new Client(13353, m_Ip));
                LOGGER.log("Client created");
            } else {
                LOGGER.log("Client already created");
            }
        }

        void deleteClient() {
            if (s_Client) {
                s_Client->stop();
                s_Client.reset();
                LOGGER.log("Client deleted");
            }
        }

        void initServer() {
            if (!s_Server) {
                s_Server.reset(new Server(13353, sf::IpAddress::getLocalAddress()));
                LOGGER.log("Server created");
            } else {
                LOGGER.log("Server already created");
            }
        }

        void deleteServer() {
            if (s_Server) {
                s_Server->stop();
                s_Server.reset();
                LOGGER.log("Server deleted");
            }
        }

        Utils::Logger LOGGER{"Luntik"};

        sf::IpAddress m_Ip;

        sf::Clock timer;
    };
}
