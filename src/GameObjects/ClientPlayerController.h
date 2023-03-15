#pragma once

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

#include "Player.h"

namespace Luntik::GameObjects {
    class ClientPlayerController {
    public:
        ClientPlayerController(Renderer::Screens::MainGameScreen* screen, Player* player) {
            m_PlayerToControl = player;
            m_GameScreen = screen;
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
                m_GameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::RUN);
            } else {
                m_GameScreen->playerSprite->setAnimationKey(Renderer::Animations::PLAYER_ANIMATIONS::IDLE);
            }

            m_GameScreen->playerSprite->getImage()->getTransform().setPos(m_PlayerToControl->getPos());
            m_GameScreen->setCameraPos(m_PlayerToControl->getPos());
        }

    private:
        Player* m_PlayerToControl;
        Renderer::Screens::MainGameScreen* m_GameScreen;
    };
}