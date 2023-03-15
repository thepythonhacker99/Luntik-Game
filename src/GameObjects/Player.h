#pragma once

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

namespace Luntik::GameObjects {
    class Player {
    public:
        Player() {
                    
        }

        ~Player() {

        }

        void tick(float deltaTime) {
            m_Acc = Utils::vec2::normalize(m_Acc);

            m_Vel += m_Acc * Settings::PLAYER_SPEED * deltaTime;
            m_Vel -= m_Vel * Settings::FRICTION * deltaTime;
            m_Pos += m_Vel * deltaTime;
        }

        void setPos(Utils::vec2 pos) {
            m_Pos = pos;
        }

        void setAcc(Utils::vec2 acc) {
            m_Acc = acc;
        }


        Utils::vec2& getAcc() {
            return m_Acc;
        }

        Utils::vec2& getVel() {
            return m_Vel;
        }

        Utils::vec2& getPos() {
            return m_Pos;
        }

    private:
        Utils::vec2 m_Pos;
        Utils::vec2 m_Vel;
        Utils::vec2 m_Acc;
    };
}