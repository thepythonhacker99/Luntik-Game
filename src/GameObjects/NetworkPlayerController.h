#pragma once

#include "Player.h"

namespace Luntik::GameObjects {
    class NetworkPlayerController {
    public:
        NetworkPlayerController(Player* player) : m_Player(player) {

        }

        void tick(float deltaTime) {
            if (m_ShouldInterpolate) {
                m_Player->setPos(m_Player->getPos() + (m_Step * deltaTime));
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
        Player* m_Player;
    };
}