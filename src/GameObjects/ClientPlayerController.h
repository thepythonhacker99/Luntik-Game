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

            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_LEFT)) {
                m_PlayerToControl->getAcc() += { -1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_RIGHT)) {
                m_PlayerToControl->getAcc() += { 1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_UP)) {
                m_PlayerToControl->getAcc() += { 0, -1 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_DOWN)) {
                m_PlayerToControl->getAcc() += { 0, 1 };
            }
            
            if (m_PlayerToControl->getAcc() != 0) {
                s_MainGameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::RUN);
            } else {
                s_MainGameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::IDLE);
            }

            s_MainGameScreen->playerSprite->getImage()->getTransform().setPos(m_PlayerToControl->getPos());
            s_MainGameScreen->setCameraPos(m_PlayerToControl->getPos());
        }

    private:
        Player* m_PlayerToControl;
    };
}