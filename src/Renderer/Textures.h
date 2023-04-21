#pragma once

#include "../Utils/Image.h"
#include "../Utils/TextureMap.h"
#include "../settings.h"

#include "SFML/Graphics.hpp"

namespace Luntik::Renderer::Textures {
    inline Utils::TextureMap::TextureMap* s_TextureMap;

    inline sf::Texture* s_BlockTexture;
    inline sf::Texture* s_WaterTexture;
    inline sf::Texture* s_TreeTexture;

    inline sf::Texture* s_PlayerTexture_idle_1;
    inline sf::Texture* s_PlayerTexture_idle_2;
    inline sf::Texture* s_PlayerTexture_idle_3;
    inline sf::Texture* s_PlayerTexture_idle_4;

    inline sf::Texture* s_PlayerTexture_run_1;
    inline sf::Texture* s_PlayerTexture_run_2;
    inline sf::Texture* s_PlayerTexture_run_3;
    inline sf::Texture* s_PlayerTexture_run_4;

    inline sf::Texture* getTextureFromFile(const std::string& filename) {
        sf::Texture* texture = new sf::Texture();
        texture->loadFromFile(filename);
        return texture;
    }

    inline void loadTextures() {
        s_TextureMap = new Utils::TextureMap::TextureMap(getTextureFromFile("images/textures.png"), Utils::vec2(Settings::ATLAS_SIZE, Settings::ATLAS_SIZE));

        s_BlockTexture  = s_TextureMap->getTexture(4, 0);
        s_WaterTexture = s_TextureMap->getTexture(4, 1);
        s_TreeTexture = s_TextureMap->getTexture(5, 0, 2, 2);

        s_PlayerTexture_idle_1 = s_TextureMap->getTexture(0, 0);
        s_PlayerTexture_idle_2 = s_TextureMap->getTexture(1, 0);
        s_PlayerTexture_idle_3 = s_TextureMap->getTexture(2, 0);
        s_PlayerTexture_idle_4 = s_TextureMap->getTexture(3, 0);

        s_PlayerTexture_run_1 = s_TextureMap->getTexture(0, 1);
        s_PlayerTexture_run_2 = s_TextureMap->getTexture(1, 1);
        s_PlayerTexture_run_3 = s_TextureMap->getTexture(2, 1);
        s_PlayerTexture_run_4 = s_TextureMap->getTexture(3, 1);
    }

    inline void unloadTextures() {
        delete s_TextureMap;

        delete s_BlockTexture;
        delete s_WaterTexture;
        delete s_TreeTexture;

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