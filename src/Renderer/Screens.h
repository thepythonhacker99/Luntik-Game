#pragma once

#include "RenderObjects/Sprite.h"
#include "RenderObjects/AnimatedSprite.h"
#include "RenderObjects/Text.h"
#include "RenderObjects/Button.h"
#include "RenderObjects/TextBox.h"
#include "RenderObjects/RenderedPlayer.h"

#include "Screen.h"

#include "Textures.h"
#include "Fonts.h"
#include "Animations.h"

#include "../Utils/vec2.h"

#include "SFML/Graphics.hpp"

#include <memory>

namespace Luntik::Renderer::Screens {
    class MainGameScreen : public Screen {
    public:
        MainGameScreen() {
            ID = MAIN_GAME_SCREEN;

            setCameraPos(Utils::vec2 {
                0,
                0
            });

            if (!amongusShader.loadFromFile("shaders/amongus.frag", sf::Shader::Fragment)) {
                std::cerr << "Failed to load BG shader" << std::endl;
                std::exit(1);
            }

            amongusShader.setUniform("texture", sf::Shader::CurrentTexture);
            amongusShader.setUniform("bodyColor", sf::Vector3f(255.f, 0.f, 0.f));

            blockSprite = std::make_unique<RenderObjects::Sprite>(Textures::s_BlockTexture, Utils::vec2{ Settings::BLOCK_SIZE, Settings::BLOCK_SIZE });
            blockSprite->getImage()->getTransform().setAlignment({ Utils::Alignment::MIDDLE, Utils::Alignment::MIDDLE });

            renderedPlayer = std::make_unique<RenderObjects::RenderedPlayer>(&amongusShader);

            text = std::make_unique<RenderObjects::Text>(
                "Luntik",
                Utils::Transform(
                    Utils::vec2{ 0, 0 },
                    Utils::vec2{ 0, 0 },
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::MIDDLE
                    }
                ),
                Fonts::s_NormalFont,
                16
            );
        }

        void render(float deltaTime) override {
            blockSprite->render(deltaTime);

            for (auto& [id, otherRenderedPlayer] : otherPlayers) {
                otherRenderedPlayer->render(deltaTime);
            }

            renderedPlayer->render(deltaTime);
        }

        void addOtherPlayer(uint32_t id, GameObjects::PlayerInfo* otherPlayerInfo) {
            if (otherPlayers.find(id) == otherPlayers.end()) {
                otherPlayers[id] = std::make_unique<RenderObjects::RenderedPlayer>(&amongusShader, otherPlayerInfo);
            }
        }

        void removeOtherPlayer(uint32_t id) {
            otherPlayers.erase(id);
        }

        std::unordered_map<uint32_t, std::unique_ptr<RenderObjects::RenderedPlayer>> otherPlayers;
        std::unique_ptr<RenderObjects::RenderedPlayer> renderedPlayer;
        std::unique_ptr<RenderObjects::Sprite> blockSprite;
        std::unique_ptr<RenderObjects::Text> text;

    private:
        sf::Shader amongusShader;
    };

    class IntroScreen : public Screen {
    public:
        IntroScreen() {
            ID = INTRO_SCREEN;

            setCameraPos(Utils::vec2{
                0,
                0
            });

            title = std::make_unique<RenderObjects::Text>(
                "Luntik",
                Utils::Transform(
                    Utils::vec2{ 0, -30 },
                    Utils::vec2{ 0, 0 },
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::FRONT
                    }
                ),
                Fonts::s_NormalFont,
                32
            );

            joinButton = std::make_unique<RenderObjects::Button>(
                "JOIN",
                Fonts::s_NormalFont,
                Utils::Transform(
                    Utils::vec2(0, 17),
                    Utils::vec2(130, 25),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::BACK
                    }
                )
            );

            hostButton = std::make_unique<RenderObjects::Button>(
                "HOST",
                Fonts::s_NormalFont,
                Utils::Transform(
                    Utils::vec2(0, 47),
                    Utils::vec2(130, 25),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::BACK
                    }
                )
            );

            textBox = std::make_unique<RenderObjects::TextBox>(
                Fonts::s_NormalFont,
                Utils::Transform(
                    Utils::vec2(0, 12),
                    Utils::vec2(130, 25),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::FRONT
                    }
                ),
                10
            );
        }

        void render(float deltaTime) override {
            title->render(deltaTime);
            joinButton->render(deltaTime);
            hostButton->render(deltaTime);
            textBox->render(deltaTime);
        }

        std::unique_ptr<RenderObjects::Text> title;
        std::unique_ptr<RenderObjects::Button> joinButton;
        std::unique_ptr<RenderObjects::Button> hostButton;
        std::unique_ptr<RenderObjects::TextBox> textBox;
    };

    class DisconnectedScreen : public Screen {
    public:
        DisconnectedScreen() {
            ID = DISCONNECTED_SCREEN;

            setCameraPos(Utils::vec2{
                0,
                0
            });

            disconnectedText = std::make_unique<RenderObjects::Text>(
                "You got disconnected",
                Utils::Transform(
                    Utils::vec2{ 0, -10 },
                    Utils::vec2{ 0, 0 },
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::FRONT
                    }
                ),
                Fonts::s_NormalFont,
                20
            );

            backButton = std::make_unique<RenderObjects::Button>(
                "BACK",
                Fonts::s_NormalFont,
                Utils::Transform(
                    Utils::vec2(0, 10),
                    Utils::vec2(120, 20),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::BACK
                    }
                )
            );
        }

        void render(float deltaTime) override {
            disconnectedText->render(deltaTime);
            backButton->render(deltaTime);
        }

        std::unique_ptr<RenderObjects::Text> disconnectedText;
        std::unique_ptr<RenderObjects::Button> backButton;
    };

    class PauseScreen : public Screen {
    public:
        PauseScreen() {
            ID = PAUSE_SCREEN;

            setCameraPos(Utils::vec2{
                0,
                0
            });

            pauseText = std::make_unique<RenderObjects::Text>(
                "Paused",
                Utils::Transform(
                    Utils::vec2{ 0, -40 },
                    Utils::vec2{ 0, 0 },
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::MIDDLE
                    }
                ),
                Fonts::s_NormalFont,
                32
            );

            backToGameButton = std::make_unique<RenderObjects::Button>(
                "BACK",
                Fonts::s_NormalFont,
                Utils::Transform(
                    Utils::vec2(0, 15),
                    Utils::vec2(130, 25),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::FRONT
                    }
                )
            );

            mainMenuButton = std::make_unique<RenderObjects::Button>(
                "TO MAIN MENU",
                Fonts::s_NormalFont,
                Utils::Transform(
                    Utils::vec2(0, 25),
                    Utils::vec2(130, 25),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::BACK
                    }
                )
            );
        }

        void render(float deltaTime) override {
            pauseText->render(deltaTime);
            backToGameButton->render(deltaTime);
            mainMenuButton->render(deltaTime);
        }

        std::unique_ptr<RenderObjects::Text> pauseText;
        std::unique_ptr<RenderObjects::Button> backToGameButton;
        std::unique_ptr<RenderObjects::Button> mainMenuButton;
    };
}
