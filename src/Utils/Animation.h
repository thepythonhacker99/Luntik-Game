#pragma once

#include "SFML/Graphics.hpp"

namespace Luntik::Utils {
    struct Animation {
        float switchTime = 0.5f;
        std::vector<sf::Texture*> frames;
    };
}