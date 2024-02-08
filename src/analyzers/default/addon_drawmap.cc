/***************************************************************
 * \file       addon_generatemap.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"
#include "map/map_parser.h"

// \todo 高版本的DE录像似乎不能得到正确的初始视角，需要修复

void DefaultAnalyzer::DrawMap(const std::string &save_path, uint32_t width, uint32_t height, bool hd) {
    FILE *mapfile_ptr;
    mapfile_ptr = fopen(save_path.c_str(), "wb");
    if (mapfile_ptr == NULL) {
        throw std::string("Failed to create map file.");
    }
    DrawMap(mapfile_ptr, width, height, hd);
    fclose(mapfile_ptr);
}

void DefaultAnalyzer::DrawMap(FILE *dest, uint32_t width, uint32_t height, bool hd) {
    Analyze2Map();

    if (7 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, DETile1>(dest, this, width, height, hd);
    } else if (9 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, DETile2>(dest, this, width, height, hd);
    } else if (4 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, Tile1>(dest, this, width, height, hd);
    } else if (2 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, TileLegacy>(dest, this, width, height, hd);
    } else {
        throw std::string("Unknown map tile type.");
    }
}