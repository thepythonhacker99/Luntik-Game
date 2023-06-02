#pragma once

#include <SFML/Graphics.hpp>

#include <unordered_map>

#include "../RenderedObject.h"
#include "../../Utils/KeySystem.h"

namespace Luntik::Renderer::RenderObjects {
    inline const std::unordered_map<sf::Keyboard::Key, char> LETTERS_KEY_MAP = {
        { sf::Keyboard::Key::A, 'A' },
        { sf::Keyboard::Key::B, 'B' },
        { sf::Keyboard::Key::C, 'C' },
        { sf::Keyboard::Key::D, 'D' },
        { sf::Keyboard::Key::E, 'E' },
        { sf::Keyboard::Key::F, 'F' },
        { sf::Keyboard::Key::G, 'G' },
        { sf::Keyboard::Key::H, 'H' },
        { sf::Keyboard::Key::I, 'I' },
        { sf::Keyboard::Key::J, 'J' },
        { sf::Keyboard::Key::K, 'K' },
        { sf::Keyboard::Key::L, 'L' },
        { sf::Keyboard::Key::M, 'M' },
        { sf::Keyboard::Key::N, 'N' },
        { sf::Keyboard::Key::O, 'O' },
        { sf::Keyboard::Key::P, 'P' },
        { sf::Keyboard::Key::Q, 'Q' },
        { sf::Keyboard::Key::R, 'R' },
        { sf::Keyboard::Key::S, 'S' },
        { sf::Keyboard::Key::T, 'T' },
        { sf::Keyboard::Key::U, 'U' },
        { sf::Keyboard::Key::V, 'V' },
        { sf::Keyboard::Key::W, 'W' },
        { sf::Keyboard::Key::X, 'X' },
        { sf::Keyboard::Key::Y, 'Y' },
        { sf::Keyboard::Key::Z, 'Z' }
    };

    inline const std::unordered_map<sf::Keyboard::Key, char> NUMBERS_KEY_MAP = {
        { sf::Keyboard::Key::Num0, '0' },
        { sf::Keyboard::Key::Num1, '1' },
        { sf::Keyboard::Key::Num2, '2' },
        { sf::Keyboard::Key::Num3, '3' },
        { sf::Keyboard::Key::Num4, '4' },
        { sf::Keyboard::Key::Num5, '5' },
        { sf::Keyboard::Key::Num6, '6' },
        { sf::Keyboard::Key::Num7, '7' },
        { sf::Keyboard::Key::Num8, '8' },
        { sf::Keyboard::Key::Num9, '9' },
    };

    constexpr char BACKSPACE = '\r';
    constexpr char DASH = '-';

    class TextBox : public RenderedObject {
    public:
        TextBox(BitmapFont* font, Utils::Transform transform, uint32_t maxSize=0,bool allowLetters=true, bool allowNumbers=true, bool allowDash=true) :
            m_RenderText(
                "",
                Utils::Transform(
                    Utils::vec2(transform.getPosWithAlignment().x + 5, transform.getMiddle().y),
                    Utils::vec2(),
                    Utils::Alignment::Alignment2D {
                        Utils::Alignment::BACK,
                        Utils::Alignment::MIDDLE
                    }
                ),
                font,
                transform.getRawSize().y - 10
            )
        {
            m_Font = font;
            m_Transform = transform;

            m_AllowLetters = allowLetters;
            m_AllowNumbers = allowNumbers;
            m_AllowDash = allowDash;

            m_MaxSize = maxSize;

            m_BoxBG.setOutlineColor(sf::Color::Red);
            m_BoxBG.setOutlineThickness(3);
            m_BoxBG.setFillColor(sf::Color::Transparent);
            m_BoxBG.setPosition(m_Transform.getPosWithAlignment());
            m_BoxBG.setSize(m_Transform.getRawSize());
        }

        ~TextBox() {

        }

        void updateText() {
            if (!m_Focused) return;

            if (Utils::KeySystem::s_KeySystem->mouseLeft == Utils::KeySystem::JUST_PRESSED) {
                if (!isPressed(m_Transform)) m_Focused = false;
            }

            if (Utils::KeySystem::s_KeySystem->keyState(sf::Keyboard::Key::Escape) == Utils::KeySystem::JUST_PRESSED) {
                m_Focused = false;
                return;
            }

            if (Utils::KeySystem::s_KeySystem->keyState(sf::Keyboard::Key::Backspace) == Utils::KeySystem::JUST_PRESSED) {
                updateInternalText(BACKSPACE);
            }

            if (m_AllowLetters) {
                for (auto& [key, str] : LETTERS_KEY_MAP) {
                    if (Utils::KeySystem::s_KeySystem->keyState(key) == Utils::KeySystem::JUST_PRESSED) {
                        updateInternalText(str);
                    }
                }
            }

            if (m_AllowNumbers) {
                for (auto& [key, str] : NUMBERS_KEY_MAP) {
                    if (Utils::KeySystem::s_KeySystem->keyState(key) == Utils::KeySystem::JUST_PRESSED) {
                        updateInternalText(str);
                    }
                }
            }

            if (m_AllowDash && Utils::KeySystem::s_KeySystem->keyState(sf::Keyboard::Key::Hyphen) == Utils::KeySystem::JUST_PRESSED) {
                updateInternalText(DASH);
            }
        }

        void render(float deltaTime) override {
            if (isPressed(m_Transform)) {
                m_Focused = true;
            }

            updateText();

            if (m_Focused) m_BoxBG.setOutlineColor(sf::Color::Green);
            else m_BoxBG.setOutlineColor(sf::Color::Red);
            s_Renderer->getWindow()->getSFMLWindow()->draw(m_BoxBG);

            if (m_Focused) m_RenderText.setText(m_RenderText.getText() + "_");
            m_RenderText.render(deltaTime);
            if (m_Focused) m_RenderText.setText(m_RenderText.getText().erase(m_RenderText.getText().size() - 1));
        }

        void setText(const std::string& text) { m_Text = text; }
        const std::string& getText() const { return m_Text; }
    private:
        void updateInternalText(char c) {
            if (c != BACKSPACE) {
                if (m_Text.size() < m_MaxSize || m_MaxSize == 0) m_Text += c;
            }
            else if (m_Text.size() > 0) m_Text.erase(m_Text.size() - 1);

            m_RenderText.setText(m_Text);
        }

        Utils::Transform m_Transform;
        BitmapFont* m_Font;

        sf::RectangleShape m_BoxBG;

        std::string m_Text = "";
        Text m_RenderText;

        uint32_t m_MaxSize;

        bool m_AllowLetters;
        bool m_AllowNumbers;
        bool m_AllowDash;

        bool m_Focused = false;
    };
}