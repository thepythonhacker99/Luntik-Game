#pragma once

#include "../Window.h"
#include "../RenderedObject.h"

#include "../../Utils/Image.h"

#include "../../GameState.h"

namespace Luntik::Renderer::RenderObjects {
    class Sprite : public RenderedObject {
    public:
        Sprite(Utils::Image* image=nullptr, sf::Shader* shader=nullptr) {
            this->m_Image = image;
            this->m_Shader = shader;
        }

        Sprite(sf::Texture* texture, Utils::vec2 stretchSize={ 0, 0 }, sf::Shader* shader=nullptr) {
            this->m_Image = new Utils::Image(texture, stretchSize);
            this->m_Shader = shader;
        }

        ~Sprite() {
            delete m_Image;
        }

        void render(float deltaTime) override {
            if (m_Image);
                s_Renderer->getWindow()->getSFMLWindow()->draw(m_Image->asSFMLSprite(), m_Shader);
        }

        Utils::Image* getImage() { return m_Image; }
    
    private:
        sf::Shader* m_Shader;
        Utils::Image* m_Image;
    };
}