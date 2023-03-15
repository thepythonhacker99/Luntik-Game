#pragma once

#include "Window.h"
#include "RenderedObject.h"
#include "Screen.h"

#include <memory>

namespace Luntik::Renderer {
    class Renderer {
    public:
        Renderer() {
            m_Window = std::make_unique<Window>("Luntik");
        }

        ~Renderer() {

        }

        bool render(float deltaTime) {
            for (auto event = sf::Event{}; m_Window->getSFMLWindow()->pollEvent(event);)
            {
                if (event.type == sf::Event::Closed)
                {
                    m_Window->getSFMLWindow()->close();
                    return true;
                }
            }

            m_Window->fixAspect();
            m_Window->getCamera()->moveTowards(getScreen()->getCameraPos(), deltaTime * Settings::SCREEN_SPEED);
            m_Window->getSFMLWindow()->clear(sf::Color::Magenta);

            if (m_Screen) {
                m_Screen->render(m_Window.get(), deltaTime);
            }

            m_Window->getSFMLWindow()->display();

            return false;
        }

        Window* getWindow() {
            return m_Window.get();
        }

        void setScreen(Screen* screen) {
            m_Screen = screen;
            m_Window->getCamera()->setCenter(m_Screen->getCameraPos());
        }

        Screen* getScreen() const {
            return m_Screen;
        }

    private:
        Screen* m_Screen;
        std::unique_ptr<Window> m_Window;
    };
}
