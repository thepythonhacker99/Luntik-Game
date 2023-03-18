#pragma once

#include <vector>
#include <unordered_map>

#include "../Window.h"
#include "../../Utils/Image.h"
#include "../../Utils/Animation.h"
#include "Sprite.h"

#include "SFML/Graphics.hpp"

namespace Luntik::Renderer::RenderObjects {
    class AnimatedSprite : public Sprite {
    public:
        AnimatedSprite(Utils::vec2 stretchSize, std::unordered_map<int, Utils::Animation> textures)
            : Sprite(nullptr, stretchSize)
        {
            m_Textures = textures;
            time = 0.f;
        }

        void update(float deltaTime) {
            time += deltaTime;

            if (time >= m_Textures.at(m_AnimationKey).switchTime) {
                time = 0.f;
                frame++;

                if (frame > m_Textures.size() - 1) {
                    frame = 0;
                }
            }
        }

        void render(float deltaTime) override {
            if (m_Textures.empty()) return;

            update(deltaTime);
            getImage()->setTexture(m_Textures.at(m_AnimationKey).frames.at(frame));

            if (getImage()) s_Renderer->getWindow()->getSFMLWindow()->draw(getImage()->asSFMLSprite());
        }

        int getAnimationKey() {
            return m_AnimationKey;
        }

        void setAnimationKey(int animationKey) {
            if (m_AnimationKey == animationKey) return;
            frame = 0;
            time = 0;
            m_AnimationKey = animationKey;
        }

    private:
        std::unordered_map<int, Utils::Animation> m_Textures;
        int frame = 0;
        float time;

        int m_AnimationKey;
    };
}