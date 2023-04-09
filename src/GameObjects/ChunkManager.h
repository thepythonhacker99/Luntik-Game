#pragma once

#include "ChunkInfo.h"
#include "../Renderer/RenderObjects/ChunkRenderer.h"

#include <vector>
#include <cmath>

namespace Luntik::GameObjects {
    class ChunkManager {
    public:
        ChunkManager() {
            
            updateVisibleChunks();
        }

        ~ChunkManager() {

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

    private:
        ChunkPos getCameraTopLeftChunkPos() {
            int chunkSizeInPx = ChunkSize * Settings::BLOCK_SIZE;
            Utils::vec2 cameraTopLeft = Utils::vec2(s_Renderer->getWindow()->getCamera()->getCenter()) - Utils::vec2(s_Renderer->getWindow()->getCamera()->getSize()) / 2;
            return ChunkPos(std::floor(cameraTopLeft.x / chunkSizeInPx), std::floor(cameraTopLeft.y / chunkSizeInPx));
        }

        void updateVisibleChunks() {
            int chunkSizeInPx = ChunkSize * Settings::BLOCK_SIZE;
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
                } while (currentChunk.x * chunkSizeInPx < cameraBottomRight.x + chunkSizeInPx);
                currentChunk.y += 1;
                currentChunk.x = cameraTopLeftChunk.x;
            } while (currentChunk.y * chunkSizeInPx < cameraBottomRight.y + chunkSizeInPx);
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
                m_Chunks.end());


            for (ChunkPos& pos : m_VisibleChunks) {
                bool found = false;

                for (ChunkInfo& chunk : m_Chunks) {
                    if (chunk.pos == pos) {
                        found = true;
                    }
                }

                if (!found) {
                    ChunkInfo newChunk(pos);
                    for (int x = 0; x < ChunkSize; x++) {
                        for (int y = 0; y < ChunkSize; y++) {
                            BlockType type = GRASS;
                            const double f = 50;
                            double noise = s_PerlinNoise->unsignedFBM((newChunk.pos.x * ChunkSize + x) / f, (newChunk.pos.y * ChunkSize + y) / f, 7, 2.1042, 0.575);

                            if (noise > 0.6) {
                                type = WATER;
                            }
                            if (noise < 0.35) {
                                type |= TREE;
                            }

                            newChunk.set(x, y, type);
                        }
                    }

                    m_Chunks.push_back(newChunk);
                }
            }
        }

        ChunkPos oldCameraTopLeftChunkPos{0, 0};

        std::vector<ChunkPos> m_VisibleChunks;
        std::vector<ChunkInfo> m_Chunks;
    };
}