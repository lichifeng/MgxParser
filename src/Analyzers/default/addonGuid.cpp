/**
 * \file       addonGuid.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-23
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "Analyzer.h"
#include "md5/md5.h"

/**
 * \brief      Guid is presented in HD/DE records, but not in older versions. Guid can be used to identify different views of same game.
 * \details    This function will combine some stable info among different views of same game and do a md5 hashing to get a uniform code
 * for them. It is used mainly in records of older versions which have no native guid embeded in record file.
 * MgxParser uses following data to generate guid of a game (* means optional):
 *     | versionStr
 *     | saveVersion
 *     | logVersion
 *     | scenarioVersion
 *     | mapDataPtr
 *     | mapSize
 *     | popLimit
 *     | gameSpeed
 *     | PlayerName + civID + index + slot + colorID + teamID by slot sequence
 *     | First 20 bytes of first MOVE action data in body
 *     | Happen time of first MOVE action
 * Note: Duration is normally different among records of same game, 'cuz player
 *       quit at different time. That's why duration cannot be a factor of this
 *       and only the very early action data can be used.
 *
 * \return     string              Guid of this record
 */
void DefaultAnalyzer::_genRetroGuid(int debugFlag)
{
    _debugFlag = debugFlag;

    vector<uint8_t> input;
    uint8_t outputBuf[16];
    MD5_CTX ctx;

    md5_init(&ctx);
    md5_update(&ctx, (BYTE *)versionStr, 8);
    md5_update(&ctx, (BYTE *)&saveVersion, 4);
    md5_update(&ctx, (BYTE *)&logVersion, 4);
    md5_update(&ctx, (BYTE *)&scenarioVersion, 4);
    md5_update(&ctx, (BYTE *)&mapSize, 4);
    md5_update(&ctx, (BYTE *)&populationLimit, 4);
    md5_update(&ctx, (BYTE *)&gameSpeed, 4);
    for (auto &p : players)
    {
        if (!p.valid())
            continue;
        md5_update(&ctx, (BYTE *)p.name.data(), p.name.size());
        md5_update(&ctx, &p.civID, 1);
        md5_update(&ctx, (BYTE *)&p.index, 4);
        md5_update(&ctx, (BYTE *)&p.slot, 4);
        md5_update(&ctx, &p.colorID, 1);
        md5_update(&ctx, &p.resolvedTeamID, 1);
        md5_update(&ctx, _firstMoveCmd, 20);
        md5_update(&ctx, (BYTE *)&_firstMoveTime, 4);
    }
    md5_update(&ctx, (BYTE *)mapDataPtr, mapCoord[0] * mapCoord[1] * _mapTileType);
    md5_final(&ctx, outputBuf);

    // Raw map data seems have some slite difference, but generated map file have md5 digest.
    // ofstream mapout("test.dat", ofstream::binary);
    // mapout.write((char *)mapDataPtr, mapCoord[0] * mapCoord[1] * _mapTileType);
    // mapout.close();

    //generateMap("map.jpg", mapCoord[0], mapCoord[1]);

    retroGuid = hexStr(outputBuf, 16);
}