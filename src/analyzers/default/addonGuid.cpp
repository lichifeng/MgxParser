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

#include "analyzer.h"
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
    const uint8_t *bufRef = outputBuf;
    MGXPARSER_MD5::MD5_CTX ctx;

    MGXPARSER_MD5::md5_init(&ctx);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)versionStr, 8);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&saveVersion, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&logVersion, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&scenarioVersion, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&mapSize, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&populationLimit, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&gameSpeed, 4);
    MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&mapID, 4); // \note Not in AOK, need a stable initialization
    for (size_t i = 0; i < _earlyMoveCnt; i++)
    {
        MGXPARSER_MD5::md5_update(&ctx, _earlyMoveCmd[i], 20);
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&_earlyMoveTime[i], 4);
    }
    for (auto &p : players)
    {
        if (!p.valid())
            continue;
        // \todo Encoding-related operations will change name bytes, caution!
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)p.name.data(), p.name.size());
        MGXPARSER_MD5::md5_update(&ctx, &p.civID, 1);
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&p.index, 4);
        MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)&p.slot, 4);
        MGXPARSER_MD5::md5_update(&ctx, &p.colorID, 1);
        MGXPARSER_MD5::md5_update(&ctx, &p.resolvedTeamID, 1);
    }
    // \note Raw map data seems have some slite difference, but generated map file have md5 digest.
    //MGXPARSER_MD5::md5_update(&ctx, (uint8_t *)mapDataPtr, mapCoord[0] *
    //mapCoord[1] * _mapTileType);
    // ofstream mapout("test.dat", ofstream::binary);
    // mapout.write((char *)mapDataPtr, mapCoord[0] * mapCoord[1] * _mapTileType);
    // mapout.close();

    MGXPARSER_MD5::md5_final(&ctx, outputBuf);

    retroGuid = hexStr(bufRef, 16);
}