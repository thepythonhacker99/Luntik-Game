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

        bool pressed() {
            return isPressed(m_Transform);
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

        Utils::Transform getTransform() const { return m_Transform; }

        void render(float deltaTime) override {
            sf::RectangleShape shape;
            shape.setSize(m_Transform.getRawSize());
            shape.setFillColor(sf::Color::Red);
            shape.setPosition(m_Transform.getPosWithAlignment());

            s_Renderer->getWindow()->getSFMLWindow()->draw(shape);

            m_Text->setPos(m_Transform.getMiddle());
            m_Text->render(deltaTime);
        }
    
    private:
        std::unique_ptr<Text> m_Text;
        Utils::Transform m_Transform;
    };
}