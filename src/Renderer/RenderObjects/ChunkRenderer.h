#pragma once

#include "../../GameObjects/ChunkInfo.h"
#include "../Textures.h"

#include "../RenderedObject.h"
#include "Sprite.h"

namespace Luntik::Renderer::RenderObjects {
    inline Sprite* s_GrassSprite;
    inline Sprite* s_WaterSprite;
    inline Sprite* s_TreeSprite;

    inline void initSprites() {
        s_GrassSprite = new Sprite(Textures::s_BlockTexture, { Settings::BLOCK_SIZE, Settings::BLOCK_SIZE });
        s_WaterSprite = new Sprite(Textures::s_WaterTexture, { Settings::BLOCK_SIZE, Settings::BLOCK_SIZE });
        s_TreeSprite = new Sprite(Textures::s_TreeTexture, { Settings::BLOCK_SIZE, Settings::BLOCK_SIZE });
    }

    inline void uninitSprites() {
        delete s_GrassSprite;
        delete s_WaterSprite;
        delete s_TreeSprite;
    }

    class ChunkRenderer : public RenderedObject {
    public:
        ChunkRenderer(GameObjects::ChunkInfo* chunkInfo) :
            m_Chunk(chunkInfo)
        {}

        ~ChunkRenderer() {
            
        }

        void render(float deltaTime) override {
            render(m_Chunk, deltaTime);
        }

        static void render(const GameObjects::ChunkInfo* chunk, float deltaTime) {
            for (int x = 0; x < GameObjects::ChunkSize; x++) {
                for (int y = 0; y < GameObjects::ChunkSize; y++) {
                    GameObjects::BlockType blockType = chunk->at(x, y);
                    Utils::vec2 pos = Utils::vec2(chunk->pos.x, chunk->pos.y) * Settings::BLOCK_SIZE * GameObjects::ChunkSize + Utils::vec2(x, y) * Settings::BLOCK_SIZE;

                    switch (blockType & GameObjects::BlockType::BLOCK_BASE) {
                        case GameObjects::BlockType::GRASS:
                            s_GrassSprite->getImage()->setStretchSize(Utils::vec2((((Utils::Math::hash_function(pos.x * pos.y) % 2) * 2) - 1) * Settings::BLOCK_SIZE, (((Utils::Math::hash_function(pos.x * pos.y) % 2) * 2) - 1) * Settings::BLOCK_SIZE));
                            s_GrassSprite->getImage()->getTransform().setPos(pos);
                            s_GrassSprite->render(deltaTime);
                            break;
                        
                        case GameObjects::BlockType::WATER:
                            s_WaterSprite->getImage()->setStretchSize(Utils::vec2((((Utils::Math::hash_function(pos.x * pos.y) % 2) * 2) - 1) * Settings::BLOCK_SIZE, (((Utils::Math::hash_function(pos.x * pos.y) % 2) * 2) - 1) * Settings::BLOCK_SIZE));
                            s_WaterSprite->getImage()->getTransform().setPos(pos);
                            s_WaterSprite->render(deltaTime);
                            break;
                        
                        case GameObjects::BlockType::STONE:
                            // render stone
                            break;
                    }

                    switch (blockType & GameObjects::BlockType::ON_BLOCK) {
                        case GameObjects::BlockType::TREE:
                            s_TreeSprite->getImage()->getTransform().setPos(pos);
                            s_TreeSprite->render(deltaTime);
                            break;

                        case GameObjects::BlockType::PLANK:
                            // render plank
                            break;
                    }
                }
            }
        }

        GameObjects::ChunkInfo* getChunk() { return m_Chunk; }
        void setChunk(GameObjects::ChunkInfo* chunk) { m_Chunk = chunk; }
    
    private:
        GameObjects::ChunkInfo* m_Chunk;
    };
}