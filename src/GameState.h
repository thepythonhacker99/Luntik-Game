#pragma once

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
    static std::unique_ptr<Renderer::Renderer> s_Renderer;
    static std::unique_ptr<Client> s_Client;
    static std::unique_ptr<Server> s_Server;

    static std::unique_ptr<Renderer::Screens::MainGameScreen> s_MainGameScreen;
    static std::unique_ptr<Renderer::Screens::IntroScreen> s_IntroScreen;
    static std::unique_ptr<Renderer::Screens::DisconnectedScreen> s_DisconnectedScreen;
    static std::unique_ptr<Renderer::Screens::PauseScreen> s_PauseScreen;
}

namespace Luntik::GameState {
    static void uninitGameState() {
        s_Renderer.reset();
        s_Client.reset();
        s_Server.reset();

        s_MainGameScreen.reset();
        s_IntroScreen.reset();
        s_DisconnectedScreen.reset();
        s_PauseScreen.reset();
    }
}