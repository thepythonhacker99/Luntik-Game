#pragma once

#include "../Utils/Image.h"
#include "../Utils/TextureMap.h"
#include "../settings.h"

#include "SFML/Graphics.hpp"

namespace Luntik::Renderer::Textures {
    static Utils::TextureMap::TextureMap* s_TextureMap;

    static sf::Texture* s_BlockTexture;

    static sf::Texture* s_PlayerTexture_idle_1;
    static sf::Texture* s_PlayerTexture_idle_2;
    static sf::Texture* s_PlayerTexture_idle_3;
    static sf::Texture* s_PlayerTexture_idle_4;

    static sf::Texture* s_PlayerTexture_run_1;
    static sf::Texture* s_PlayerTexture_run_2;
    static sf::Texture* s_PlayerTexture_run_3;
    static sf::Texture* s_PlayerTexture_run_4;

    static sf::Texture* getTextureFromFile(const std::string& filename) {
        sf::Texture* texture = new sf::Texture();
        texture->loadFromFile(filename);
        return texture;
    }

    static void loadTextures() {
        s_TextureMap = new Utils::TextureMap::TextureMap(getTextureFromFile("images/textures.png"), Utils::vec2(Settings::BLOCK_SIZE, Settings::BLOCK_SIZE));

        s_BlockTexture  = s_TextureMap->getTexture(4, 0);

        s_PlayerTexture_idle_1 = s_TextureMap->getTexture(0, 0);
        s_PlayerTexture_idle_2 = s_TextureMap->getTexture(1, 0);
        s_PlayerTexture_idle_3 = s_TextureMap->getTexture(2, 0);
        s_PlayerTexture_idle_4 = s_TextureMap->getTexture(3, 0);

        s_PlayerTexture_run_1 = s_TextureMap->getTexture(0, 1);
        s_PlayerTexture_run_2 = s_TextureMap->getTexture(1, 1);
        s_PlayerTexture_run_3 = s_TextureMap->getTexture(2, 1);
        s_PlayerTexture_run_4 = s_TextureMap->getTexture(3, 1);
    }

    static void unloadTextures() {
        delete s_TextureMap;
        delete s_BlockTexture;

        delete s_PlayerTexture_idle_1;
        delete s_PlayerTexture_idle_2;
        delete s_PlayerTexture_idle_3;
        delete s_PlayerTexture_idle_4;

        delete s_PlayerTexture_run_1;
        delete s_PlayerTexture_run_2;
        delete s_PlayerTexture_run_3;
        delete s_PlayerTexture_run_4;
    }
}