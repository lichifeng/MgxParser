/**
 * \file       subProcMapData.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"
#include "MapTools/MapTools.h"

using namespace std;

void DefaultAnalyzer::AnalyzeMap(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(map_start_) >> mapCoord;
    if (mapCoord[0] >= 10000 || mapCoord[1] >= 10000) {
        throw std::string("Map size too large.");
    } else if (mapCoord[0] != mapCoord[1]) {
        throw std::string("Map coordinates is weird.");
    }

    int32_t num_mapzones, map_bits, num_floats;
    map_bits = mapCoord[0] * mapCoord[1];
    cursor_ >> num_mapzones;
    for (int i = 0; i < num_mapzones; i++) {
        if (IS_HD(version_code_) || IS_DE(version_code_)) /// \todo 为什么不是11.76？
            cursor_ >> (2048 + map_bits * 2);
        else
            cursor_ >> (1275 + map_bits);
        cursor_ >> num_floats >> (num_floats * 4 + 4);
    }
    cursor_ >> allVisible >> 1; // fog of war

    mapdata_ptr_ = cursor_.Ptr();

    uint32_t checkVal = *(uint32_t *) (_curPos + 7 * map_bits);
    if (IS_DE(version_code_)) {
        _mapTileType = (save_version_ >= 13.0299 || checkVal > 1000) ? 9 : 7;
    } else {
        _mapTileType = (_curPos[0] == 255) ? 4 : 2;
    }
    _skip(_mapTileType * map_bits);

    int32_t numData, numObstructions;
    _readBytes(4, &numData);
    _skip(4 + numData * 4);
    for (int i = 0; i < numData; i++) {
        _readBytes(4, &numObstructions);
        _skip(numObstructions * 8);
    }
    int32_t visibilityMapSize[2];
    _readBytes(8, visibilityMapSize);
    _skip(visibilityMapSize[0] * visibilityMapSize[1] * 4);
}

///< \todo 高版本的DE录像好像不能得到正确的初始视角，需要修复
void DefaultAnalyzer::generateMap(const string path, uint32_t width, uint32_t height, bool hd) {
    if (7 == _mapTileType) {
        return getMap<DefaultAnalyzer, DETile1>(path, this, width, height, hd);
    } else if (9 == _mapTileType) {
        return getMap<DefaultAnalyzer, DETile2>(path, this, width, height, hd);
    } else if (4 == _mapTileType) {
        return getMap<DefaultAnalyzer, Tile1>(path, this, width, height, hd);
    } else if (2 == _mapTileType) {
        return getMap<DefaultAnalyzer, TileLegacy>(path, this, width, height, hd);
    } else {
        logger_->warn(
                "{}(): Unknown _mapTileType.",
                __FUNCTION__);
        _sendExceptionSignal();
        return;
    }
}