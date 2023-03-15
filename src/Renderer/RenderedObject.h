#pragma once

#include "Window.h"

namespace Luntik::Renderer {
    class RenderedObject {
    public:
        virtual void render(Luntik::Renderer::Window* window, float deltaTime) {}
    };
}