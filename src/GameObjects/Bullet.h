#pragma once

#include "../Utils/vec2.h"

namespace Luntik::GameObjects {
    constexpr float BULLET_ALIVE_TIME = 3.f;

    class Bullet {
    public:
        Bullet(float damage, float speed, Utils::vec2 dir, Utils::vec2 pos, Network::ID shooterId, std::unordered_map<Network::ID, GameObjects::PlayerInfo>* players) : m_Damage(damage), m_Speed(speed), m_Dir(dir), m_Pos(pos), m_Shooter(shooterId), m_Players(players) {

        }

        void tick(float deltaTime) {
            if (!m_Alive) return;

            m_Time += deltaTime;
            m_Pos += m_Dir * m_Speed * deltaTime;

            for (auto& [id, player] : *m_Players) {
                if (id == m_Shooter) continue;

                if (
                    Utils::AABB(
                        m_Pos, Utils::vec2{2, 2},
                        player.pos, Utils::vec2{Settings::BLOCK_SIZE, Settings::BLOCK_SIZE}
                    )
                ) {
                    m_Alive = false;
                    player.setHealth(player.health - m_Damage);
                }
            }

            if (m_Time >= BULLET_ALIVE_TIME) {
                m_Alive = false;
            }
        }

        bool getAlive() const { return m_Alive; }
        void setAlive(bool value) { m_Alive = value; }

    private:
        bool m_Alive = true;

        Network::ID m_Shooter;
        std::unordered_map<Network::ID, GameObjects::PlayerInfo>* m_Players;

        float m_Time = 0.f;

        float m_Speed;
        Utils::vec2 m_Dir;
        Utils::vec2 m_Pos;

        float m_Damage;
    };
}