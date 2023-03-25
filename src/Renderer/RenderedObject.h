#pragma once

#include "../Utils/Transform.h"
#include "../Utils/KeySystem.h"

#include "../Renderer/Renderer.h"

namespace Luntik::Renderer {
    class RenderedObject {
    public:
        static bool isPressed(const Utils::Transform& transform) {
            if (Utils::KeySystem::s_KeySystem->mouseLeft == Utils::KeySystem::JUST_PRESSED) {
                Utils::vec2 mousePos = Utils::vec2(Luntik::s_Renderer->getWindow()->getSFMLWindow()->mapPixelToCoords(sf::Mouse::getPosition(*s_Renderer->getWindow()->getSFMLWindow()))) + Utils::vec2(s_Renderer->getWindow()->getCamera()->getCenter());
                Utils::vec2 dPos = transform.getPosWithAlignment() + Utils::vec2(Luntik::s_Renderer->getWindow()->getCamera()->getCenter());

                if (mousePos.x > dPos.x && mousePos.x < dPos.x + transform.getRawSize().x) {
                    if (mousePos.y > dPos.y && mousePos.y < dPos.y + transform.getRawSize().y) {
                        return true;
                    }
                }
            }
            return false;
        }

        virtual void render(float deltaTime) {}
    };
}