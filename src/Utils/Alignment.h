#pragma once

namespace Luntik::Utils::Alignment {
    enum Alignment {
        BACK,
        MIDDLE,
        FRONT
    };

    struct Alignment2D {
        Alignment x = BACK;
        Alignment y = BACK;

        operator vec2() {
            return vec2(x, y);
        }
    };
}