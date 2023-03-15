#pragma once

#include "Transform.h"
#include "../Renderer/Textures.h"

#include "SFML/Graphics.hpp"

#include <memory>
#include <iostream>

namespace Luntik::Utils {
    class Image {
    public:
        Image(sf::Texture* texture, Utils::vec2 stretchSize={ 0, 0 }) {
            this->m_StretchSize = stretchSize;
            this->m_Texture = texture;

            if (stretchSize == 0 && texture) {
                this->m_Transform.setSize(vec2(texture->getSize()));
            } else {
                this->m_Transform.setSize(stretchSize);
            }
        }

        sf::Sprite asSFMLSprite() {
            sf::Sprite sprite(*m_Texture);
            sprite.setPosition(m_Transform.getPosWithAlignment());
            if (m_StretchSize != 0) sprite.setScale(m_StretchSize / vec2(m_Texture->getSize()));
            return sprite;
        }

        sf::Texture* getTexture() {
            return m_Texture;
        }

        void setTexture(sf::Texture* texture) {
            m_Texture = texture;
            
            if (m_StretchSize == 0 && texture) {
                this->m_Transform.setSize(vec2(texture->getSize()));
            } else {
                this->m_Transform.setSize(m_StretchSize);
            }
        }

        Transform& getTransform() { return m_Transform; }
        void setTransform(Transform transform) { m_Transform = transform; }

        vec2 getStretchSize() const { return m_StretchSize; }
        void setStretchSize(vec2 newStretchSize) { m_StretchSize = newStretchSize; if (newStretchSize != 0) this->m_Transform.setSize(newStretchSize); else this->m_Transform.setSize(vec2(m_Texture->getSize())); }

    private:
        sf::Texture* m_Texture;
        Transform m_Transform;
        vec2 m_StretchSize;
    };
}