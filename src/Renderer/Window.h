#pragma once

#include "Camera.h"

#include "SFML/Graphics.hpp"

#include <memory>

namespace Luntik::Renderer {
    class Window {
    public:
        Window(std::string windowName) : m_WindowName(windowName) {
            m_Camera = new Camera();
            m_Window = new sf::RenderWindow(sf::VideoMode(1280, 720), m_WindowName, sf::Style::Default);

            m_Window->setView(*m_Camera);

            m_FullScreenVideoMode = sf::VideoMode::getFullscreenModes()[0];
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

        bool getFullScreen() const { return m_FullScreen; }

        void setFullScreen(bool fullScreen) {
            m_FullScreen = fullScreen;
            initWindow();
        }

        void toggleFullScreen() {
            m_FullScreen = !m_FullScreen;
            initWindow();
        }

    private:
        void initWindow() {
            if (!m_FullScreen) {
                m_Window->create(sf::VideoMode(1280, 720), m_WindowName, sf::Style::Default);
            } else {
                m_Window->create(m_FullScreenVideoMode, m_WindowName, sf::Style::Fullscreen);
            }

            fixAspect();
        }

        bool m_FullScreen = false;

        sf::RenderWindow* m_Window;
        Camera* m_Camera;

        std::string m_WindowName;

        sf::VideoMode m_FullScreenVideoMode;
    };
}