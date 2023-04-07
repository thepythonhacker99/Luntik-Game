#pragma once

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

namespace Luntik::GameObjects {
    struct PlayerInfo {
        sf::Vector3f color = sf::Vector3f(-1, -1, -1); // (0 -> 1) range
        Utils::vec2 pos;
        Utils::vec2 acc;
        std::string name = "\n";

        bool isReady() {
            return
                name != "\n" &&
                color != sf::Vector3f(-1, -1, -1);
        }
    };
}