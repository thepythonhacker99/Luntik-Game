#pragma once

#include "SFML/Window.hpp"
#include <unordered_map>

#include "../Renderer/Window.h"

namespace Luntik::Utils::KeySystem {
    class KeySystem;

    static KeySystem* s_KeySystem;

    enum KeyState {
        NOT_PRESSED,
        JUST_PRESSED,
        PRESSED
    };
    
    class KeySystem {
        public:
            KeySystem(Renderer::Window* window) : m_Window(window) {}

            KeyState keyState(sf::Keyboard::Key key) {
                if (keys.find(key) == keys.end())
                    keys[key] = sf::Keyboard::isKeyPressed(key) == true ? JUST_PRESSED : NOT_PRESSED;
                
                return keys.at(key);
            }

            void update() {
                for (auto& [key, wasPressed] : keys) {
                    if (m_Window->getSFMLWindow()->hasFocus()) {
                        switch (wasPressed) {
                            case NOT_PRESSED:
                                keys[key] = sf::Keyboard::isKeyPressed(key) == true ? JUST_PRESSED : NOT_PRESSED;
                                break;

                            case JUST_PRESSED:
                                keys[key] = sf::Keyboard::isKeyPressed(key) == true ? PRESSED : NOT_PRESSED;
                                break;

                            case PRESSED:
                                keys[key] = sf::Keyboard::isKeyPressed(key) == true ? PRESSED : NOT_PRESSED;
                                break;
                        }
                    } else {
                        keys[key] = NOT_PRESSED;
                    }
                }

                if (m_Window->getSFMLWindow()->hasFocus()) {
                    switch (mouseLeft) {
                        case NOT_PRESSED:
                            mouseLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left) == true ? JUST_PRESSED : NOT_PRESSED;
                            break;
                        case JUST_PRESSED:
                            mouseLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left) == true ? PRESSED : NOT_PRESSED;
                            break;
                        case PRESSED:
                            mouseLeft = sf::Mouse::isButtonPressed(sf::Mouse::Left) == true ? PRESSED : NOT_PRESSED;
                            break;
                    }

                    switch (mouseRight) {
                        case NOT_PRESSED:
                            mouseRight = sf::Mouse::isButtonPressed(sf::Mouse::Left) == true ? JUST_PRESSED : NOT_PRESSED;
                            break;
                        case JUST_PRESSED:
                            mouseRight = sf::Mouse::isButtonPressed(sf::Mouse::Left) == true ? PRESSED : NOT_PRESSED;
                            break;
                        case PRESSED:
                            mouseRight = sf::Mouse::isButtonPressed(sf::Mouse::Left) == true ? PRESSED : NOT_PRESSED;
                            break;
                    }
                } else {
                    mouseLeft = NOT_PRESSED;
                    mouseRight = NOT_PRESSED;
                }
            }

            std::unordered_map<sf::Keyboard::Key, KeyState> keys;
            KeyState mouseLeft = NOT_PRESSED;
            KeyState mouseRight = NOT_PRESSED;
        
        private:
            Renderer::Window* m_Window;
    };

    static void initializeKeySystem(Renderer::Window* window) {
        s_KeySystem = new KeySystem(window);
    }

}
