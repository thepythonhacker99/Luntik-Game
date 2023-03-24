#pragma once

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

namespace Luntik::GameObjects {
    struct PlayerInfo {
        Utils::vec2 pos;
        Utils::vec2 acc;
    };
}