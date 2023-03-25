#pragma once

#include "RenderedObject.h"
#include "../Utils/vec2.h"

#include <vector>
#include <iostream>

namespace Luntik::Renderer::Screens {
    enum SCREENS_ID {
        DEFAULT,
        MAIN_GAME_SCREEN,
        INTRO_SCREEN,
        DISCONNECTED_SCREEN,
        PAUSE_SCREEN
    };
}

namespace Luntik::Renderer {
    class Screen {
    public:
        Screen() {

        }

        ~Screen() {

        }

        virtual void render(float deltaTime) {}

        void setCameraPos(Utils::vec2 pos) {
            this->m_CameraPos = pos;
        }

        Utils::vec2 getCameraPos() const {
            return this->m_CameraPos;
        }

        Screens::SCREENS_ID ID = Screens::DEFAULT;

    private:
        Utils::vec2 m_CameraPos;
    };
}