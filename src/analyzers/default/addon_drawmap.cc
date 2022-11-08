/***************************************************************
 * \file       addon_generatemap.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"
#include "map/map_parser.h"

// \todo 高版本的DE录像似乎不能得到正确的初始视角，需要修复

void DefaultAnalyzer::DrawMap(const std::string &save_path, uint32_t width, uint32_t height, bool hd) {
    if (nullptr == mapdata_ptr_)
        return;

    if (7 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, DETile1>(save_path, this, width, height, hd);
    } else if (9 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, DETile2>(save_path, this, width, height, hd);
    } else if (4 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, Tile1>(save_path, this, width, height, hd);
    } else if (2 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, TileLegacy>(save_path, this, width, height, hd);
    } else {
        throw std::string("Unknown map tile type.");
    }
}