#pragma once

#include "ChunkInfo.h"
#include "BasicChunkManager.h"

#include "../Renderer/RenderObjects/ChunkRenderer.h"

#include <vector>
#include <cmath>

namespace Luntik::GameObjects {
    class ChunkManager : public BasicChunkManager {
    public:
        ChunkManager() {
            updateVisibleChunks();
        }

        ~ChunkManager() {

        }
    private:
        void onGenerateChunk(ChunkInfo& chunk) {
            generateChunk(chunk);
        }
    };
}