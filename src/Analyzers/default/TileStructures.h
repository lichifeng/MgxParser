#pragma once
#pragma pack(push)  // 保存对齐状态 
#pragma pack(1)     // 设定为4字节对齐

#include <cstdint>

struct DETile1 {
    uint8_t     terrainType;
    uint8_t     padding;
    uint8_t     elevation;
    int16_t     unknown0;
    int16_t     unknown1;
};

struct DETile2 {
    uint8_t     terrainType;
    uint8_t     padding;
    uint8_t     elevation;
    int16_t     unknown0;
    int16_t     unknown1;
    int16_t     unknown2;
};

struct Tile1 {
    uint8_t     identifier;
    uint8_t     terrainType;
    uint8_t     elevation;
    uint8_t     padding;
};

struct TileLegacy {
    uint8_t     terrainType;
    uint8_t     elevation;
};

#pragma pack(pop)   // 恢复对齐状态