#pragma once

#include "ADDONS/modules/BitmapFont/BitmapFont.hpp"

namespace Luntik::Renderer::Fonts {
    static BitmapFont* s_NormalFont;

    static void loadFonts() {
        s_NormalFont = new BitmapFont();
        s_NormalFont->loadFromFile("images/font.png");
    }

    static void unloadFonts() {
        delete s_NormalFont;
    }
}