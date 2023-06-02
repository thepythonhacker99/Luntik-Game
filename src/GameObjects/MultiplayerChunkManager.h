#pragma once

#include "ChunkInfo.h"
#include "BasicChunkManager.h"

#include "../Networking/Packets.h"

#include "../Renderer/RenderObjects/ChunkRenderer.h"

#include <vector>
#include <cmath>

namespace Luntik::GameObjects {
    class MultiplayerChunkManager : public BasicChunkManager {
    public:
        MultiplayerChunkManager() {
            updateVisibleChunks();
        }

        ~MultiplayerChunkManager() {

        }

        void handleNewChunkInfo(const ChunkInfo& chunkInfo) {
            for (ChunkInfo& chunk : m_Chunks) {
                if (chunkInfo.pos == chunk.pos) {
                    memcpy(chunk.blocks, chunkInfo.blocks, sizeof(ChunkInfo::blocks));
                    break;
                }
            }
        }

    private:
        void sendChunkPacket(const ChunkInfo& chunk) {
            if (!s_ClientSocket) return;
            sf::Packet chunkPacket = Network::Packets::createC2SChunkPacket({ chunk.pos });
            s_ClientSocket->send(chunkPacket);
        }

        void onGenerateChunk(ChunkInfo& chunk) override {
            sendChunkPacket(chunk);
        }
    };
}