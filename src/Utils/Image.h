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
            setTexture(texture);

            if (stretchSize == 0 && texture) {
                this->m_Transform.setSize(vec2(texture->getSize()));
            } else {
                this->m_Transform.setSize(stretchSize);
            }
        }

        sf::Sprite asSFMLSprite() {
            m_SfmlSprite.setPosition(m_Transform.getPosWithAlignment());

            if (m_StretchSize != 0) {
                m_SfmlSprite.setScale(m_StretchSize / vec2(m_Texture->getSize()));
                if (m_StretchSize.x < 0) m_SfmlSprite.setPosition(Utils::vec2(m_SfmlSprite.getPosition()) + Utils::vec2{ -m_StretchSize.x, 0 });
                if (m_StretchSize.y < 0) m_SfmlSprite.setPosition(Utils::vec2(m_SfmlSprite.getPosition()) + Utils::vec2{ 0, -m_StretchSize.y });
            }

            return m_SfmlSprite;
        }

        sf::Texture* getTexture() {
            return m_Texture;
        }

        void setTexture(sf::Texture* texture) {
            this->m_Texture = texture;

            if (texture) {
                this->m_SfmlSprite.setTexture(*texture);
            }
            
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
        sf::Sprite m_SfmlSprite;
        sf::Texture* m_Texture;
        Transform m_Transform;
        vec2 m_StretchSize;
    };
}