#pragma once

#include "SFML/Graphics.hpp"

namespace Luntik::Settings {
    static const sf::Keyboard::Key PLAYER_LEFT = sf::Keyboard::A;
    static const sf::Keyboard::Key PLAYER_RIGHT = sf::Keyboard::D;
    static const sf::Keyboard::Key PLAYER_UP = sf::Keyboard::W;
    static const sf::Keyboard::Key PLAYER_DOWN = sf::Keyboard::S;

    static const int HEIGHT = 180;
    static const float SCREEN_SPEED = 5.f;

    static const float PLAYER_SPEED = 500.f;
    static const float FRICTION = 7.f;

    static const float BLOCK_SIZE = 16.f;

    static const int SEND_POS_RATE = 30;
}