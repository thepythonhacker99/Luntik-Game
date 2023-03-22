#pragma once

#include "Player.h"
#include "../GameState.h"
#include "../Renderer/Animations.h"

namespace Luntik::GameObjects {
    class NetworkPlayerController {
    public:
        NetworkPlayerController(Player* player, uint32_t id) : m_Player(player), m_Id(id) {

        }

        void tick(float deltaTime) {
            if (m_ShouldInterpolate) {
                m_Player->setPos(m_Player->getPos() + (m_Step * deltaTime));
                s_MainGameScreen->otherPlayers.at(m_Id)->getImage()->setStretchSize(Utils::vec2((int(m_Step.x > 0) * 2 - 1) * Settings::BLOCK_SIZE, Settings::BLOCK_SIZE));

                if (m_Step == 0) {
                    s_MainGameScreen->otherPlayers.at(m_Id)->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::IDLE);
                } else {
                    s_MainGameScreen->otherPlayers.at(m_Id)->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::RUN);
                }
            }
        }

        void setGoal(Utils::vec2 goal) {
            m_Goal = goal;
            m_Step = (m_Goal - m_Player->getPos()) * Settings::SEND_POS_RATE;

            m_ShouldInterpolate = true;
        }

        void setPlayer(Player* player) {
            m_Player = player;
        }

    private:
        bool m_ShouldInterpolate = false;
        Utils::vec2 m_Goal;
        Utils::vec2 m_Step;
        uint32_t m_Id;
        Player* m_Player;
    };
}