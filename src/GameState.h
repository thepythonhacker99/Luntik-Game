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
}

namespace Luntik {
    static Renderer::Renderer* s_Renderer;
    static Client* s_Client;
    static Server* s_Server;

    static Renderer::Screens::MainGameScreen* s_MainGameScreen;
    static Renderer::Screens::IntroScreen* s_IntroScreen;
    static Renderer::Screens::DisconnectedScreen* s_DisconnectedScreen;
}