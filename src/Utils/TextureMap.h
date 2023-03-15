#pragma once

#include "vec2.h"

#include "SFML/Graphics.hpp"

#include <string>

namespace Luntik::Utils::TextureMap {
    class TextureMap {
    public:
        TextureMap(sf::Texture* texture, int imagesOnX, int imagesOnY) {
            this->m_Texture = texture;
            this->m_ImagesOnX = imagesOnX;
            this->m_ImagesOnY = imagesOnY;

            this->m_ImageSize = vec2(texture->getSize()) / vec2(imagesOnX, imagesOnY);
        }

        TextureMap(sf::Texture* texture, Utils::vec2 imageSize) {
            this->m_Texture = texture;
            this->m_ImageSize = imageSize;

            this->m_ImagesOnX = texture->getSize().x / imageSize.x;
            this->m_ImagesOnY = texture->getSize().y / imageSize.y;
        }

        sf::IntRect getTextureRect(int xIndex, int yIndex) {
            return sf::IntRect(int(m_ImageSize.x * (float)xIndex), int(m_ImageSize.y * (float)yIndex), int(m_ImageSize.x), int(m_ImageSize.y));
        }

        sf::Texture* getTexture(int xIndex, int yIndex) {
            sf::Texture* texture = new sf::Texture();
            texture->loadFromImage(m_Texture->copyToImage(), sf::IntRect(int(m_ImageSize.x * (float)xIndex), int(m_ImageSize.y * (float)yIndex), (int)m_ImageSize.x, (int)m_ImageSize.y));
            return std::move(texture);
        }

        sf::Texture* getMapTexture() {
            return m_Texture;
        }

    private:
        sf::Texture* m_Texture;

        vec2 m_ImageSize;
        int m_ImagesOnX;
        int m_ImagesOnY;
    };
}