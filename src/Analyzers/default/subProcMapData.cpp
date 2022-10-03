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

#include "Analyzer.h"
#include "../../MapTools/MapTools.h"

using namespace std;

void DefaultAnalyzer::_mapDataAnalyzer()
{
    _readBytes(8, &mapCoord);
    if (mapCoord[0] >= 10000 || mapCoord[1] >= 10000)
    {
        logger->warn("Abnormal mapsize. \"{}\"", filename);
        _sendFailedSignal();
        return;
    }
    else if (mapCoord[0] != mapCoord[1])
    {
        logger->warn("Map coordinates is weird, X != Y. \"{}\"", filename);
        _sendFailedSignal();
        return;
    }

    int32_t numMapZones, mapBits, numFloats;
    mapBits = mapCoord[0] * mapCoord[1];
    _readBytes(4, &numMapZones);
    for (int i = 0; i < numMapZones; i++)
    {
        if (IS_HD(versionCode) || IS_DE(versionCode)) /// \todo 为什么不是11.76？
            _skip(2048 + mapBits * 2);
        else
            _skip(1275 + mapBits);
        _readBytes(4, &numFloats);
        _skip(numFloats * 4 + 4);
    }

    _readBytes(1, &allVisible);
    _skip(1); //_readBytes(1, &fogOfWar); // Use fogOfWar in lobby

    mapDataPtr = _curPos;
    uint32_t checkVal = *(uint32_t *)(_curPos + 7 * mapBits);
    if (IS_DE(versionCode))
    {
        _mapTileType = (saveVersion >= 13.0299 || checkVal > 1000) ? 9 : 7;
    }
    else
    {
        _mapTileType = (_curPos[0] == 255) ? 4 : 2;
    }
    _skip(_mapTileType * mapBits);

    int32_t numData, numObstructions;
    _readBytes(4, &numData);
    _skip(4 + numData * 4);
    for (int i = 0; i < numData; i++)
    {
        _readBytes(4, &numObstructions);
        _skip(numObstructions * 8);
    }
    int32_t visibilityMapSize[2];
    _readBytes(8, visibilityMapSize);
    _skip(visibilityMapSize[0] * visibilityMapSize[1] * 4);
}

///< \todo 高版本的DE录像好像不能得到正确的初始视角，需要修复
void DefaultAnalyzer::generateMap(const string path, uint32_t width, uint32_t height, bool hd)
{
    if (7 == _mapTileType)
    {
        return getMap<DefaultAnalyzer, DETile1>(path, this, width, height, hd);
    }
    else if (9 == _mapTileType)
    {
        return getMap<DefaultAnalyzer, DETile2>(path, this, width, height, hd);
    }
    else if (4 == _mapTileType)
    {
        return getMap<DefaultAnalyzer, Tile1>(path, this, width, height, hd);
    }
    else if (2 == _mapTileType)
    {
        return getMap<DefaultAnalyzer, TileLegacy>(path, this, width, height, hd);
    }
    else
    {
        logger->warn(
            "{}(): Unknown _mapTileType. @{}.",
            __FUNCTION__, _distance());
        _sendFailedSignal();
        return;
    }
}