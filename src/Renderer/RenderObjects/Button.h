#pragma once

#include "SFML/Graphics.hpp"

#include "../Window.h"
#include "../RenderedObject.h"

#include "../../Utils/Transform.h"
#include "../../Utils/KeySystem.h"

#include "Text.h"

#include "ADDONS/modules/BitmapFont/BitmapFont.hpp"

#include <string>

namespace Luntik::Renderer::RenderObjects {
    class Button : public RenderedObject {
    public:
        Button(const std::string& text, BitmapFont* font, Utils::Transform transform) {
            this->m_Transform = transform;

            this->m_Text = std::make_unique<Text>(
                text,
                Utils::Transform(
                    Utils::vec2(0, 0),
                    transform.getRawSize(),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::MIDDLE,
                        Utils::Alignment::MIDDLE
                    }
                ),
                font,
                transform.getRawSize().y / 1.5
            );
        }

        ~Button() {
            
        }

        bool isPressed(Window* window) {
            if (Utils::KeySystem::s_KeySystem->mouseLeft == Utils::KeySystem::JUST_PRESSED) {
                Utils::vec2 mousePos = Utils::vec2(window->getSFMLWindow()->mapPixelToCoords(sf::Mouse::getPosition(*window->getSFMLWindow()))) + Utils::vec2(window->getCamera()->getCenter());
                Utils::vec2 dPos = m_Transform.getPosWithAlignment() + Utils::vec2(window->getCamera()->getCenter());

                if (mousePos.x > dPos.x && mousePos.x < dPos.x + m_Transform.getRawSize().x) {
                    if (mousePos.y > dPos.y && mousePos.y < dPos.y + m_Transform.getRawSize().y) {
                        return true;
                    }
                }
            }
            return false;
        }

        void setPos(Utils::vec2 pos) {
            m_Transform.setPos(pos);
        }
        Utils::vec2 getRawPos() {
            return m_Transform.getRawPos();
        }

        void setSize(Utils::vec2 size) {
            m_Transform.setSize(size);
        }
        Utils::vec2 getRawSize() {
            return m_Transform.getRawSize();
        }

        void render(Window* window, float deltaTime) override {
            sf::RectangleShape shape;
            shape.setSize(m_Transform.getRawSize());
            shape.setFillColor(sf::Color::Red);
            shape.setPosition(m_Transform.getPosWithAlignment());

            window->getSFMLWindow()->draw(shape);

            m_Text->setPos(m_Transform.getMiddle());
            m_Text->render(window, deltaTime);
        }
    
    private:
        std::unique_ptr<Text> m_Text;
        Utils::Transform m_Transform;
    };
}