#pragma once

#include "SimplexNoise.hpp"

namespace Luntik {
    class Client;
    class Server;
}

namespace Luntik::Renderer {
    class Renderer;
}

namespace Luntik::Renderer::Screens {
    class MainGameScreen;
    class IntroScreen;
    class DisconnectedScreen;
    class PauseScreen;
}

namespace Luntik {
    inline std::unique_ptr<Renderer::Renderer> s_Renderer;
    inline std::unique_ptr<Client> s_Client;
    inline std::unique_ptr<Server> s_Server;

    inline std::unique_ptr<Renderer::Screens::MainGameScreen> s_MainGameScreen;
    inline std::unique_ptr<Renderer::Screens::IntroScreen> s_IntroScreen;
    inline std::unique_ptr<Renderer::Screens::DisconnectedScreen> s_DisconnectedScreen;
    inline std::unique_ptr<Renderer::Screens::PauseScreen> s_PauseScreen;

    inline std::unique_ptr<SimplexNoise> s_PerlinNoise;
}

namespace Luntik::GameState {
    inline void uninitGameState() {
        s_Renderer.reset();
        s_Client.reset();
        s_Server.reset();
        s_PerlinNoise.reset();

        s_MainGameScreen.reset();
        s_IntroScreen.reset();
        s_DisconnectedScreen.reset();
        s_PauseScreen.reset();
    }
}