#pragma once

#include "RenderObjects/Sprite.h"
#include "RenderObjects/AnimatedSprite.h"
#include "RenderObjects/Text.h"
#include "RenderObjects/Button.h"

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

            blockSprite = std::make_unique<RenderObjects::Sprite>(Textures::s_BlockTexture, Utils::vec2{ Settings::BLOCK_SIZE, Settings::BLOCK_SIZE });
            blockSprite->getImage()->getTransform().setAlignment({ Utils::Alignment::MIDDLE, Utils::Alignment::MIDDLE });

            playerSprite = std::make_unique<RenderObjects::AnimatedSprite>(Utils::vec2{ Settings::BLOCK_SIZE, Settings::BLOCK_SIZE }, Animations::s_PlayerAnimation);
            playerSprite->setAnimationKey(Animations::PLAYER_ANIMATIONS::IDLE);
            playerSprite->getImage()->getTransform().setAlignment({ Utils::Alignment::MIDDLE, Utils::Alignment::MIDDLE });

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

            for (auto& [id, sprite] : otherPlayers) {
                sprite->render(deltaTime);
            }

            playerSprite->render(deltaTime);
        }

        void addOtherPlayer(uint32_t id) {
            if (otherPlayers.count(id) == 0) {
                otherPlayers[id] = std::make_unique<RenderObjects::AnimatedSprite>(Utils::vec2{ Settings::BLOCK_SIZE, Settings::BLOCK_SIZE }, Animations::s_PlayerAnimation);
                otherPlayers.at(id)->setAnimationKey(Animations::PLAYER_ANIMATIONS::IDLE);
                otherPlayers.at(id)->getImage()->getTransform().setAlignment({ Utils::Alignment::MIDDLE, Utils::Alignment::MIDDLE });
            }
        }

        void removeOtherPlayer(uint32_t id) {
            otherPlayers.erase(id);
        }

        std::unordered_map<uint32_t, std::unique_ptr<RenderObjects::AnimatedSprite>> otherPlayers;
        std::unique_ptr<RenderObjects::AnimatedSprite> playerSprite;
        std::unique_ptr<RenderObjects::Sprite> blockSprite;
        std::unique_ptr<RenderObjects::Text> text;
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

            joinButton = std::make_unique<RenderObjects::Button>(
                "JOIN",
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

            hostButton = std::make_unique<RenderObjects::Button>(
                "HOST",
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
            title->render(deltaTime);
            joinButton->render(deltaTime);
            hostButton->render(deltaTime);
        }

        std::unique_ptr<RenderObjects::Text> title;
        std::unique_ptr<RenderObjects::Button> joinButton;
        std::unique_ptr<RenderObjects::Button> hostButton;
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
}
