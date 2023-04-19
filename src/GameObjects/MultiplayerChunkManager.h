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
        MultiplayerChunkManager(sf::TcpSocket* socket) : m_Socket(socket) {
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

        void setSocket(sf::TcpSocket* socket) { m_Socket = socket; }
        sf::TcpSocket* getSocket() const { return m_Socket; }

    private:
        void sendChunkPacket(const ChunkInfo& chunk) {
            if (!m_Socket) return;
            sf::Packet chunkPacket = Network::Packets::createC2SChunkPacket({ chunk.pos });
            m_Socket->send(chunkPacket);
        }

        void onGenerateChunk(ChunkInfo& chunk) override {
            sendChunkPacket(chunk);
        }

        sf::TcpSocket* m_Socket;
    };
}