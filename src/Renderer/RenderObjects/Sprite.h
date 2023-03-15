#pragma once

#include "../Window.h"
#include "../RenderedObject.h"

#include "../../Utils/Image.h"

namespace Luntik::Renderer::RenderObjects {
    class Sprite : public RenderedObject {
    public:
        Sprite(Utils::Image* image=nullptr) {
            this->m_Image = image;
        }

        Sprite(sf::Texture* texture, Utils::vec2 stretchSize={ 0, 0 }) {
            this->m_Image = new Utils::Image(texture, stretchSize);
        }

        ~Sprite() {
            delete m_Image;
        }

        void render(Window* window, float deltaTime) override {
            if (m_Image);
                window->getSFMLWindow()->draw(m_Image->asSFMLSprite());
        }

        Utils::Image* getImage() { return m_Image; }
    
    private:
        Utils::Image* m_Image;
    };
}