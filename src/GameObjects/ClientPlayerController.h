#pragma once

#include "../GameState.h"

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

#include "../settings.h"

#include "PlayerInfo.h"

namespace Luntik::GameObjects {
    class ClientPlayerController {
    public:
        ClientPlayerController(PlayerInfo* player) {
            m_PlayerToControl = player;
        }

        ~ClientPlayerController() {

        }

        void tick(float deltaTime) {
            m_PlayerToControl->acc = { 0, 0 };

            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_LEFT) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { -1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_RIGHT) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { 1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_UP) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { 0, -1 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_DOWN) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { 0, 1 };
            }

            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_LEFT) == Utils::KeySystem::JUST_PRESSED) {
                m_Dir = -1;
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_RIGHT) == Utils::KeySystem::JUST_PRESSED) {
                m_Dir = 1;
            }
            
            if (m_PlayerToControl->acc != 0) {
                s_MainGameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::RUN);
            } else {
                s_MainGameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::IDLE);
            }

            m_Vel += Utils::vec2::normalize(m_PlayerToControl->acc) * Settings::PLAYER_SPEED * deltaTime;
            m_Vel -= m_Vel * Settings::FRICTION * deltaTime;
            m_PlayerToControl->pos += m_Vel * deltaTime;

            s_MainGameScreen->playerSprite->getImage()->getTransform().setPos(m_PlayerToControl->pos);
            s_MainGameScreen->playerSprite->getImage()->setStretchSize(Utils::vec2(m_Dir * Settings::BLOCK_SIZE, Settings::BLOCK_SIZE));
            s_MainGameScreen->setCameraPos(m_PlayerToControl->pos);
        }

        Utils::vec2 getVel() const { return m_Vel; }
        int getDir() const { return m_Dir; }

    private:
        int m_Dir = 1;
        PlayerInfo* m_PlayerToControl;
        Utils::vec2 m_Vel;
    };
}