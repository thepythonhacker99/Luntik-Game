#pragma once

#include "../GameState.h"

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

#include "../settings.h"

#include "Player.h"

namespace Luntik::GameObjects {
    class ClientPlayerController {
    public:
        ClientPlayerController(Player* player) {
            m_PlayerToControl = player;
        }

        ~ClientPlayerController() {

        }

        void tick(float deltaTime) {
            m_PlayerToControl->setAcc({ 0, 0 });

            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_LEFT) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->getAcc() += { -1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_RIGHT) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->getAcc() += { 1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_UP) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->getAcc() += { 0, -1 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_DOWN) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->getAcc() += { 0, 1 };
            }

            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_LEFT) == Utils::KeySystem::JUST_PRESSED) {
                m_Dir = -1;
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_RIGHT) == Utils::KeySystem::JUST_PRESSED) {
                m_Dir = 1;
            }
            
            if (m_PlayerToControl->getAcc() != 0) {
                s_MainGameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::RUN);
            } else {
                s_MainGameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::IDLE);
            }

            s_MainGameScreen->playerSprite->getImage()->getTransform().setPos(m_PlayerToControl->getPos());
            s_MainGameScreen->playerSprite->getImage()->setStretchSize(Utils::vec2(m_Dir * Settings::BLOCK_SIZE, Settings::BLOCK_SIZE));
            s_MainGameScreen->setCameraPos(m_PlayerToControl->getPos());
        }

    private:
        int m_Dir = 1;
        Player* m_PlayerToControl;
    };
}