#pragma once

#include "PlayerInfo.h"
#include "../GameState.h"
#include "../Renderer/Animations.h"
#include "../Utils/Math.h"

namespace Luntik::GameObjects {
    class NetworkPlayerController {
    public:
        NetworkPlayerController(PlayerInfo* player, uint32_t id) : m_Player(player), m_Id(id) {

        }

        void tick(float deltaTime) {
            if (m_ShouldInterpolate) {
                m_Player->pos.x = Utils::Math::interpolate(m_Start.x, m_Goal.x, m_Player->pos.x, (float)(Settings::SEND_POS_RATE * deltaTime * .5));
                m_Player->pos.y = Utils::Math::interpolate(m_Start.y, m_Goal.y, m_Player->pos.y, (float)(Settings::SEND_POS_RATE * deltaTime * .5));

                if (m_Player->acc != 0) {
                    s_MainGameScreen->otherPlayers.at(m_Id)->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::RUN);
                } else {
                    s_MainGameScreen->otherPlayers.at(m_Id)->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::IDLE);
                }
                
                if (m_Player->acc.x != 0) {
                    s_MainGameScreen->otherPlayers.at(m_Id)->getImage()->setStretchSize({ Settings::BLOCK_SIZE * m_Player->acc.x, Settings::BLOCK_SIZE });
                }
            }
        }

        void setGoal(Utils::vec2 goal) {
            m_Goal = goal;
            m_Start = m_Player->pos;
            if (!m_ShouldInterpolate) {
                m_Player->pos = goal;
                m_ShouldInterpolate = true;
            }
        }

        Utils::vec2 getGoal() const { return m_Goal; }

        void setPlayer(PlayerInfo* player, uint32_t id) {
            m_Player = player;
            m_Id = id;
        }

        void setId(uint32_t id) { m_Id = id; }
        uint32_t getId() const { return m_Id; }

    private:
        bool m_ShouldInterpolate = false;
        Utils::vec2 m_Goal;
        Utils::vec2 m_Start;
        uint32_t m_Id;
        PlayerInfo* m_Player;
    };
}