#pragma once

#include <cstring>

namespace Luntik::GameObjects {
    struct ChunkPos {
        int x, y;

        ChunkPos(int x, int y) : x(x), y(y) {}
        ChunkPos() : x(0), y(0) {}

        bool operator==(const ChunkPos& other) const {
            return x == other.x && y == other.y;
        }

        bool operator!=(const ChunkPos& other) const {
            return !(*this == other);
        }
    };

    constexpr int ChunkSize = 8;
    constexpr float ChunkSizeInPx = ChunkSize * Settings::BLOCK_SIZE;

    inline ChunkPos getChunkOfPixel(Utils::vec2 pixel) {
        return ChunkPos(std::floor(pixel.x / ChunkSizeInPx), std::floor(pixel.y / ChunkSizeInPx));
    }

    inline ChunkPos getCameraTopLeftChunkPos() {
        Utils::vec2 cameraTopLeft = Utils::vec2(s_Renderer->getWindow()->getCamera()->getCenter()) - Utils::vec2(s_Renderer->getWindow()->getCamera()->getSize()) / 2;
        return getChunkOfPixel(cameraTopLeft);
    }

    enum BlockType {
        GRASS      = 0b0001 << 0,
        WATER      = 0b0010 << 0,
        STONE      = 0b0011 << 0,
        BLOCK_BASE = 0b1111 << 0,

        TREE     = 0b0001 << 4,
        PLANK    = 0b0011 << 4,
        ON_BLOCK = 0b1111 << 4,
    };

    inline BlockType  operator~  (BlockType  a)              { return (BlockType)~(int)a; }
    inline BlockType  operator|  (BlockType  a, BlockType b) { return (BlockType)((int)a | (int)b); }
    inline BlockType  operator&  (BlockType  a, BlockType b) { return (BlockType)((int)a & (int)b); }
    inline BlockType  operator^  (BlockType  a, BlockType b) { return (BlockType)((int)a ^ (int)b); }
    inline BlockType& operator|= (BlockType& a, BlockType b) { return (BlockType&)((int&)a |= (int)b); }
    inline BlockType& operator&= (BlockType& a, BlockType b) { return (BlockType&)((int&)a &= (int)b); }
    inline BlockType& operator^= (BlockType& a, BlockType b) { return (BlockType&)((int&)a ^= (int)b); }

    struct ChunkInfo {
        ChunkPos pos;
        BlockType blocks[ChunkSize * ChunkSize];

        BlockType at(int x, int y) const {
            return blocks[y * ChunkSize + x];
        }

        void set(int x, int y, BlockType block) {
            blocks[y * ChunkSize + x] = block;
        }

        ChunkInfo(ChunkPos pos) : pos(pos) {
            std::memset(blocks, 0, sizeof(BlockType) * ChunkSize * ChunkSize);
        }

        ChunkInfo() {
            std::memset(blocks, 0, sizeof(BlockType) * ChunkSize * ChunkSize);
        }
    };

    static void generateChunk(ChunkInfo& chunk) {
        for (int x = 0; x < ChunkSize; x++) {
            for (int y = 0; y < ChunkSize; y++) {
                BlockType type = GRASS;
                const double f = 50;
                double noise = s_PerlinNoise->unsignedFBM((chunk.pos.x * ChunkSize + x) / f, (chunk.pos.y * ChunkSize + y) / f, 7, 2.1042, 0.575);

                if (noise > 0.6) {
                    type = WATER;
                }
                if (noise < 0.35) {
                    type |= TREE;
                }

                chunk.set(x, y, type);
            }
        }
    }
}