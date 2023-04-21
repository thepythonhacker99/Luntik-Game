#pragma once

#include "SFML/Graphics.hpp"

namespace Luntik::Settings {
    constexpr sf::Keyboard::Key PLAYER_LEFT = sf::Keyboard::A;
    constexpr sf::Keyboard::Key PLAYER_RIGHT = sf::Keyboard::D;
    constexpr sf::Keyboard::Key PLAYER_UP = sf::Keyboard::W;
    constexpr sf::Keyboard::Key PLAYER_DOWN = sf::Keyboard::S;

    constexpr int HEIGHT = 260;
    // constexpr int HEIGHT = 720;
    constexpr float SCREEN_SPEED = 5.f;

    constexpr float PLAYER_SPEED = 1200.f;
    // constexpr float PLAYER_SPEED = 12000.f;
    // constexpr float PLAYER_SPEED = 12000000.f;
    constexpr float FRICTION = 9.f;

    constexpr float BLOCK_SIZE = 16.f;
    constexpr float ATLAS_SIZE = 16.f;

    constexpr int SEND_POS_RATE = 30;
}