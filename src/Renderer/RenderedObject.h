#pragma once

#include "Window.h"

namespace Luntik::Renderer {
    class RenderedObject {
    public:
        virtual void render(float deltaTime) {}
    };
}