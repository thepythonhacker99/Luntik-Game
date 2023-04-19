#pragma once

#include "PlayerInfo.h"
#include "../GameState.h"
#include "../Renderer/Animations.h"
#include "../Utils/Math.h"

namespace Luntik::GameObjects {
    constexpr float InterpolationMultiplier = 1.5f;

    class NetworkPlayerController {
    public:
        NetworkPlayerController(PlayerInfo* player, uint32_t id) : m_Player(player), m_Id(id) {

        }

        void tick(float deltaTime) {
            m_Timestep += deltaTime;
            if (m_ShouldInterpolate) {
                m_Player->pos.x = Utils::Math::interpolate(m_Start.x, m_Goal.x, m_Timestep / ((1.f / Settings::SEND_POS_RATE) * InterpolationMultiplier));
                m_Player->pos.y = Utils::Math::interpolate(m_Start.y, m_Goal.y, m_Timestep / ((1.f / Settings::SEND_POS_RATE) * InterpolationMultiplier));
            }
        }

        void setGoal(Utils::vec2 goal) {
            m_Goal = goal;
            m_Start = m_Player->pos;
            m_Timestep = 0.f;
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
        uint32_t m_Id;
        bool m_ShouldInterpolate = false;
        double m_Timestep = 0.f;
        Utils::vec2 m_Goal;
        Utils::vec2 m_Start;
        PlayerInfo* m_Player;
    };
}