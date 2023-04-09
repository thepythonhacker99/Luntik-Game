#pragma once

#include <cstring>

namespace Luntik::GameObjects {
    struct ChunkPos {
        int x, y;

        ChunkPos(int x, int y) : x(x), y(y) {}

        bool operator==(const ChunkPos& other) {
            return x == other.x && y == other.y;
        }

        bool operator!=(const ChunkPos& other) {
            return !(*this == other);
        }
    };

    constexpr int ChunkSize = 8;

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
    };
}