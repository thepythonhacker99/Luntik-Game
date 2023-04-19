#pragma once

#include "ChunkInfo.h"
#include "../Renderer/RenderObjects/ChunkRenderer.h"

#include <vector>
#include <cmath>

namespace Luntik::GameObjects {
    class BasicChunkManager {
    public:
        BasicChunkManager() {
            
            updateVisibleChunks();
        }

        ~BasicChunkManager() {

        }

        void tick(float deltaTime) {
            ChunkPos newCameraTopLeftChunkPos = getCameraTopLeftChunkPos();
            if (newCameraTopLeftChunkPos != oldCameraTopLeftChunkPos) {
                updateChunks();
                oldCameraTopLeftChunkPos = newCameraTopLeftChunkPos;
            }
        }

        void render(float deltaTime) {
            for (ChunkInfo& chunk : m_Chunks) {
                Renderer::RenderObjects::ChunkRenderer::render(&chunk, deltaTime);
            }
        }

        ChunkInfo* getChunkInfoAtChunkPos(const ChunkPos& chunkPos) {
            for (ChunkInfo& chunk : m_Chunks) {
                if (chunkPos == chunk.pos) return &chunk;
            }

            return nullptr;
        }
    private:
        void updateVisibleChunks() {
            Utils::vec2 cameraTopLeft = Utils::vec2(s_Renderer->getWindow()->getCamera()->getCenter()) - Utils::vec2(s_Renderer->getWindow()->getCamera()->getSize()) / 2;
            Utils::vec2 cameraBottomRight = cameraTopLeft + Utils::vec2(s_Renderer->getWindow()->getCamera()->getSize());

            ChunkPos cameraTopLeftChunk = getCameraTopLeftChunkPos();

            m_VisibleChunks.clear();

            ChunkPos currentChunk = cameraTopLeftChunk;
            // m_VisibleChunks.push_back(currentChunk);
            do {
                do {
                    m_VisibleChunks.push_back(currentChunk);
                    currentChunk.x += 1;
                } while (currentChunk.x * ChunkSizeInPx < cameraBottomRight.x + ChunkSizeInPx);
                currentChunk.y += 1;
                currentChunk.x = cameraTopLeftChunk.x;
            } while (currentChunk.y * ChunkSizeInPx < cameraBottomRight.y + ChunkSizeInPx);
        }

        void updateChunks() {
            updateVisibleChunks();

            m_Chunks.erase(
                std::remove_if(
                    m_Chunks.begin(),
                    m_Chunks.end(),
                    [this](const ChunkInfo& chunk) {
                        return std::find(m_VisibleChunks.begin(), m_VisibleChunks.end(), chunk.pos) == m_VisibleChunks.end();
                    }
                ),
                m_Chunks.end()
            );

            for (ChunkPos& pos : m_VisibleChunks) {
                bool found = false;

                for (ChunkInfo& chunk : m_Chunks) {
                    if (chunk.pos == pos) {
                        found = true;
                    }
                }

                if (!found) {
                    ChunkInfo newChunk(pos);
                    onGenerateChunk(newChunk);

                    m_Chunks.push_back(newChunk);
                }
            }
        }
        
        virtual void onGenerateChunk(ChunkInfo& chunk) {};

        ChunkPos oldCameraTopLeftChunkPos{0, 0};

        std::vector<ChunkPos> m_VisibleChunks;
        std::vector<ChunkInfo> m_Chunks;

        friend class MultiplayerChunkManager;
        friend class ChunkManager;
    };
}