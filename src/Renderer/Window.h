#pragma once

#include "Camera.h"

#include "SFML/Graphics.hpp"

#include <memory>

namespace Luntik::Renderer {
    class Window {
    public:
        Window(const char* window_name) {
            m_Window = new sf::RenderWindow(sf::VideoMode(1280, 720), window_name);
            m_Camera = new Camera();

            m_Window->setView(*m_Camera);
        }

        ~Window() {
            delete m_Window;
            delete m_Camera;
        }

        void fixAspect() {
            m_Camera->setWidthByAspectRatio(m_Camera->getAspectRatio(m_Window->getSize().x, m_Window->getSize().y));
            m_Window->setView(*m_Camera);
        }

        sf::RenderWindow* getSFMLWindow() const {
            return m_Window;
        }

        Camera* getCamera() const {
            return m_Camera;
        }
    private:
        sf::RenderWindow* m_Window;
        Camera* m_Camera;
    };
}