#pragma once

#include "../Utils/Animation.h"
#include "Textures.h"

#include <vector>
#include <unordered_map>

namespace Luntik::Renderer::Animations {
    static std::unordered_map<int, Utils::Animation> s_PlayerAnimation;
    enum PLAYER_ANIMATIONS {
        IDLE,
        RUN
    };

    static void loadAnimations() {
        s_PlayerAnimation[PLAYER_ANIMATIONS::IDLE] = {
            0.5f,
            std::vector<sf::Texture*> {
                Textures::s_PlayerTexture_idle_1,
                Textures::s_PlayerTexture_idle_2,
                Textures::s_PlayerTexture_idle_3,
                Textures::s_PlayerTexture_idle_4
            }
        };

        s_PlayerAnimation[PLAYER_ANIMATIONS::RUN] = {
            0.35f,
            std::vector<sf::Texture*> {
                Textures::s_PlayerTexture_run_1,
                Textures::s_PlayerTexture_run_2,
                Textures::s_PlayerTexture_run_3,
                Textures::s_PlayerTexture_run_4
            }
        };
    }
}