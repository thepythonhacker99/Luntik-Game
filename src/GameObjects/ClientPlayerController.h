#pragma once

#include "../GameState.h"

#include "../Renderer/Screens.h"

#include "../Utils/KeySystem.h"
#include "../Utils/vec2.h"

#include "../settings.h"

#include "PlayerInfo.h"

namespace Luntik::GameObjects {
    inline const std::vector<Utils::vec2> playerChunkOffsets = {
        {  0,  0 },   {  0, -1 }, {  0,  1 },
        {  1,  0 },   {  1,  1 }, {  1, -1 },
        { -1, -1 },   { -1,  1 }, { -1,  1 }, 
    };

    class ClientPlayerController {
    public:
        ClientPlayerController(PlayerInfo* player) {
            m_PlayerToControl = player;
        }

        ~ClientPlayerController() {

        }

        void shoot() {
            // dir => player_pos <-> mouse_pos_including_camera
            Utils::vec2 mouse = Renderer::getMousePosRelativeToWorld() - Utils::vec2(s_Renderer->getWindow()->getCamera()->getCenter());
            Utils::vec2 dir = mouse - (m_PlayerToControl->pos + (Settings::BLOCK_SIZE / 2.f));
            std::cout << (std::string)dir << std::endl;

            // send shooting packet to the server
            sf::Packet packet = Network::Packets::createC2SShootPacket({ dir });
            s_ClientSocket->send(packet);
        }

        void tick(float deltaTime) {
            // movement
            m_PlayerToControl->acc = { 0, 0 };

            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_LEFT) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { -1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_RIGHT) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { 1, 0 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_UP) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { 0, -1 };
            }
            if (Utils::KeySystem::s_KeySystem->keyState(Settings::PLAYER_DOWN) == Utils::KeySystem::PRESSED) {
                m_PlayerToControl->acc += { 0, 1 };
            }

            m_Vel += Utils::vec2::normalize(m_PlayerToControl->acc) * Settings::PLAYER_SPEED * deltaTime;
            m_Vel -= m_Vel * Settings::FRICTION * deltaTime;

            // move on x
            m_PlayerToControl->pos.x += m_Vel.x * deltaTime;
            resolveCollisionsOnX();

            // move on y
            m_PlayerToControl->pos.y += m_Vel.y * deltaTime;
            resolveCollisionsOnY();

            s_MainGameScreen->setCameraPos(m_PlayerToControl->pos + (Settings::BLOCK_SIZE / 2.f));

            //shooting
            if (Utils::KeySystem::s_KeySystem->mouseLeft == Utils::KeySystem::JUST_PRESSED) {
                shoot();
            }
        }

        Utils::vec2 getVel() const { return m_Vel; }
        void setVel(Utils::vec2 v) { m_Vel = v; }

    private:
        void resolveCollisionsOnX() {
            if (m_Vel.x == 0) return;

            ChunkPos currentPlayerChunkPos = getChunkOfPixel(m_PlayerToControl->pos);

            for (const Utils::vec2& offset : playerChunkOffsets) {
                ChunkPos currentChunkPos(currentPlayerChunkPos.x + offset.x, currentPlayerChunkPos.y + offset.y);

                ChunkInfo* currentChunkInfo = s_MainGameScreen->chunkManager->getChunkInfoAtChunkPos(currentChunkPos);
                if (!currentChunkInfo) continue;

                for (int x = 0; x < ChunkSize; x++) {
                    for (int y = 0; y < ChunkSize; y++) {
                        BlockType blockType = currentChunkInfo->at(x, y);

                        if (blockType & WATER) {
                            // player can collide with this block
                            Utils::vec2 blockPosInPx = Utils::vec2{currentChunkPos.x * ChunkSizeInPx + x * Settings::BLOCK_SIZE, currentChunkPos.y * ChunkSizeInPx + y * Settings::BLOCK_SIZE};

                            if (Utils::AABB(m_PlayerToControl->pos + 2, Utils::vec2{Settings::BLOCK_SIZE, Settings::BLOCK_SIZE} - 4, blockPosInPx, Utils::vec2{ Settings::BLOCK_SIZE, Settings::BLOCK_SIZE })) {
                                // in collision with this block
                                if (m_Vel.x > 0) {
                                    m_PlayerToControl->pos.x = blockPosInPx.x - Settings::BLOCK_SIZE + 2;
                                } else if (m_Vel.x < 0) {
                                    m_PlayerToControl->pos.x = blockPosInPx.x + Settings::BLOCK_SIZE - 2;
                                }
                                m_Vel.x = 0;
                                return;
                            }
                        }
                    }
                }
            }
        }

        void resolveCollisionsOnY() {
            if (m_Vel.y == 0) return;

            ChunkPos currentPlayerChunkPos = getChunkOfPixel(m_PlayerToControl->pos);

            for (const Utils::vec2& offset : playerChunkOffsets) {
                ChunkPos currentChunkPos(currentPlayerChunkPos.x + offset.x, currentPlayerChunkPos.y + offset.y);
                ChunkInfo* currentChunkInfo = s_MainGameScreen->chunkManager->getChunkInfoAtChunkPos(currentChunkPos);

                if (!currentChunkInfo) continue;

                for (int x = 0; x < ChunkSize; x++) {
                    for (int y = 0; y < ChunkSize; y++) {
                        BlockType blockType = currentChunkInfo->at(x, y);

                        if (blockType & WATER) {
                            // player can collide with this block
                            Utils::vec2 blockPosInPx = Utils::vec2{currentChunkPos.x * ChunkSizeInPx + x * Settings::BLOCK_SIZE, currentChunkPos.y * ChunkSizeInPx + y * Settings::BLOCK_SIZE};

                            if (Utils::AABB(m_PlayerToControl->pos + 2, Utils::vec2{Settings::BLOCK_SIZE, Settings::BLOCK_SIZE} - 4, blockPosInPx, Utils::vec2{ Settings::BLOCK_SIZE, Settings::BLOCK_SIZE })) {
                                // in collision with this block
                                if (m_Vel.y > 0) {
                                    m_PlayerToControl->pos.y = blockPosInPx.y - Settings::BLOCK_SIZE + 2;
                                } else if (m_Vel.y < 0) {
                                    m_PlayerToControl->pos.y = blockPosInPx.y + Settings::BLOCK_SIZE - 2;
                                }
                                m_Vel.y = 0;
                                return;
                            }
                        }
                    }
                }
            }
        }

        PlayerInfo* m_PlayerToControl;
        Utils::vec2 m_Vel;
    };
}