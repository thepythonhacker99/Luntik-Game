#pragma once

#include "../../GameObjects/PlayerInfo.h"

#include "../Animations.h"
#include "../RenderedObject.h"

#include "AnimatedSprite.h"
#include "Text.h"

namespace Luntik::Renderer::RenderObjects {
    class RenderedPlayer : public RenderedObject {
    public:
        RenderedPlayer(sf::Shader* shader, GameObjects::PlayerInfo* player=nullptr) : 
            m_Player(player),
            m_Shader(shader),
            m_Sprite(
                Utils::vec2(Settings::BLOCK_SIZE, Settings::BLOCK_SIZE),
                Animations::s_PlayerAnimation,
                shader
            ),
            m_NameText(
                player ? player->name : "PLAYER",
                Utils::Transform(
                    Utils::vec2(),
                    Utils::vec2(),
                    {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::FRONT
                    }
                ),
                Fonts::s_NormalFont,
                7
            )
        {
            m_Sprite.setAnimationKey(Animations::PLAYER_ANIMATIONS::IDLE);
            m_Sprite.getImage()->getTransform().setAlignment({ Utils::Alignment::MIDDLE, Utils::Alignment::MIDDLE });
        }

        void render(float deltaTime) override {
            if (!m_Player) return;

            // PLAYER SPRITE
            if (m_Player->acc != 0) {
                m_Sprite.setAnimationKey(Animations::RUN);
            } else {
                m_Sprite.setAnimationKey(Animations::IDLE);
            }

            m_Sprite.getImage()->getTransform().setPos(m_Player->pos);

            if (m_Player->acc.x != 0) {
                m_Sprite.getImage()->setStretchSize({ Settings::BLOCK_SIZE * m_Player->acc.x, Settings::BLOCK_SIZE });
            }

            m_Shader->setUniform("bodyColor", m_Player->color);
            m_Sprite.render(deltaTime);

            // PLAYER NAME
            m_NameText.setPos(Utils::vec2{ m_Player->pos.x, m_Sprite.getImage()->getTransform().getPosWithAlignment().y });
            m_NameText.render(deltaTime);
        }

        void updateNameText() {
            if (m_Player) m_NameText.setText(m_Player->name);
        }

        void setPlayer(GameObjects::PlayerInfo* player) { 
            m_Player = player;
            updateNameText();
        }

        GameObjects::PlayerInfo* getPlayer() { return m_Player; }

    private:
        sf::Shader* m_Shader;

        AnimatedSprite m_Sprite;
        GameObjects::PlayerInfo* m_Player;

        Text m_NameText;
    };
}