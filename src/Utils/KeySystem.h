#pragma once

#include "SFML/Window.hpp"

#include "../Renderer/Renderer.h"
#include "../GameState.h"

#include <unordered_map>

namespace Luntik::Utils::KeySystem {
    class KeySystem;

    inline KeySystem* s_KeySystem;

    enum KeyState {
        NOT_PRESSED,
        JUST_PRESSED,
        PRESSED
    };
    
    class KeySystem {
        public:
            KeySystem() = default;

            KeyState keyState(sf::Keyboard::Key key) {
                if (keys.find(key) == keys.end())
                    keys[key] = sf::Keyboard::isKeyPressed(key) == true ? JUST_PRESSED : NOT_PRESSED;
                
                return keys.at(key);
            }

            void update() {
                for (auto& [key, wasPressed] : keys) {
                    if (s_Renderer->getWindow()->getSFMLWindow()->hasFocus()) {
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

                if (s_Renderer->getWindow()->getSFMLWindow()->hasFocus()) {
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
    };

    inline void initializeKeySystem() {
        s_KeySystem = new KeySystem();
    }

}
