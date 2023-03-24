#pragma once

#include "PlayerInfo.h"
#include "../GameState.h"
#include "../Renderer/Animations.h"

namespace Luntik::GameObjects {
    class NetworkPlayerController {
    public:
        NetworkPlayerController(PlayerInfo* player, uint32_t id) : m_Player(player), m_Id(id) {

        }

        void tick(float deltaTime) {
            if (m_ShouldInterpolate) {
                m_Player->pos += m_Step * deltaTime;

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
            m_Step = (m_Goal - m_Player->pos) * Settings::SEND_POS_RATE;

            m_ShouldInterpolate = true;
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
        Utils::vec2 m_Step;
        uint32_t m_Id;
        PlayerInfo* m_Player;
    };
}