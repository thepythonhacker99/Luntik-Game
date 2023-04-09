#pragma once

#include "ADDONS/modules/BitmapFont/BitmapFont.hpp"

namespace Luntik::Renderer::Fonts {
    inline BitmapFont* s_NormalFont;

    inline void loadFonts() {
        s_NormalFont = new BitmapFont();
        s_NormalFont->loadFromFile("images/font.png");
    }

    inline void unloadFonts() {
        delete s_NormalFont;
    }
}