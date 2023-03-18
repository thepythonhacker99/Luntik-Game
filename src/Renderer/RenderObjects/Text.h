#pragma once

#include "SFML/Graphics.hpp"

#include "../Window.h"
#include "../RenderedObject.h"

#include "../../GameState.h"

#include "../../Utils/Transform.h"

#include "ADDONS/modules/BitmapFont/BitmapFont.hpp"
#include "ADDONS/modules/BitmapFont/BitmapText.hpp"

#include <string>

namespace Luntik::Renderer::RenderObjects {
    class Text : public RenderedObject {
    public:
        Text(const std::string& text, Utils::Transform transform, BitmapFont* font, float height) {
            this->m_Transform = transform;

            this->m_Text = text;
            this->m_Height = height;

            this->m_Font = font;

            this->m_RenderText.setFont(*font);
            setText(text);
            setSize(height);
        }

        ~Text() {
            
        }

        std::string getText() { return m_Text; }
        void setText(const std::string& text) {
            m_Text = text; m_RenderText.setString(text);
            m_Transform.setSize(Utils::vec2(m_RenderText.getSize()));
        }

        Utils::vec2 getRawPos() {
            return m_Transform.getRawPos();
        }
        void setPos(Utils::vec2 pos) {
            m_Transform.setPos(pos);
        }

        void setScale(float scale) {
            this->m_RenderText.setScale({ scale, scale });
            this->m_Transform.setSize(Utils::vec2(m_RenderText.getSize()));
        }

        void setSize(float height) {
            this->m_RenderText.setScale({ height / m_Font->getGlyphSize().y, height / m_Font->getGlyphSize().y });
            this->m_Transform.setSize(Utils::vec2(m_RenderText.getSize()));
        }

        void render(float deltaTime) override {
            m_RenderText.setPosition(m_Transform.getPosWithAlignment());
            s_Renderer->getWindow()->getSFMLWindow()->draw(m_RenderText);
        }

        BitmapText* getDrawingText() { return &m_RenderText; }
    
    private:
        BitmapFont* m_Font;
        BitmapText m_RenderText;

        Utils::Transform m_Transform;

        std::string m_Text;
        float m_Height;
    };
}