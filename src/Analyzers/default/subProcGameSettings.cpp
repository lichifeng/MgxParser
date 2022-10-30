/**
 * \file       subProcGameSettings.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

/**
 * \brief      This method also builds some search patterns used to traverse in startinfo section
 * \todo       Game setting section is different in DE version, better look into it.
 */
void DefaultAnalyzer::_gameSettingsAnalyzer(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _curPos = _gameSettingsPos;

    _skip(64 + 4 + 8);
    if (IS_HD(versionCode) && saveVersion > 12.3401)
        _skip(16);
    if (!IS_AOK(versionCode))
        _readBytes(4, &mapID);
    _readBytes(4, &difficultyID);
    _readBytes(4, &lockTeams);
    if (IS_DE(versionCode))
    {
        _skip(29);
        if (saveVersion >= 13.0699)
            _skip(1);
        if (saveVersion >= 13.3399)
            _skip(132);
        if (saveVersion >= 20.0599)
            _skip(1);
        if (saveVersion >= 20.1599)
            _skip(4);
        if (saveVersion >= 25.0199)
            _skip(4 * 16);
        if (saveVersion >= 25.0599 && saveVersion < 26.2099)
            _skip(4);
    }

    uint16_t nameLen;
    const uint8_t *namePtr;
    for (size_t i = 0; i < 9; i++)
    {
        players[i].slot = i;
        namePtr = _curPos + 8;
        // 先获得名字长度，再把 len + lenName + trailBytes 放到 pattern 里，
        // 供以后搜索使用， 然后再跳过或把名字转码保存下来。
        // trailBytes 只保存了一个指针，具体用后面的几个字节组装可以试下，看
        // 看性能表现，一般建议在[1, 6]个，第一个一般是0x16,第六个一般是
        // 0x21，中间一个4字节的数字则根据版本不同有所区别。
        // 注1：HD/DE版本中，startinfo里的名字数据和HD/DE header里的一
        // 样，和这里不一样。这里显示的玩家名是“玩家1”之类。
        // 注2：startinfo中字符串长度用2字节表示，这里用4字节。
        // \todo 这里似乎只有玩家才会在HD/DE专有区块有名字信息，GAIA和AI没有。
        // 注3：startinfo中的字符串有\0结尾，这里没有

        _readBytes(4, &players[i].index);
        if (players[i].name.length() > 0)
        {
            // Data from HD/DE header has higher priority
            _skip(4);
            nameLen = players[i].name.length() + 1;
            players[i].searchPattern.resize(2 + nameLen + trailBytes);
            memcpy(players[i].searchPattern.data(), &nameLen, 2);
            memcpy(players[i].searchPattern.data() + 2, players[i].name.data(), nameLen);
            memcpy(players[i].searchPattern.data() + 2 + nameLen, _startInfoPatternTrail, trailBytes);
            _skipPascalString(true);
        }
        else
        {
            _readBytes(4, &players[i].type);
            // 两处字符串前面有\0，后面有的版本有，有的版本没有，需要判断处理
            nameLen = *(uint32_t *)namePtr;
            if ('\0' == *(namePtr + 4 + nameLen - 1))
            {
                players[i].searchPattern.resize(2 + nameLen + trailBytes);
                memcpy(players[i].searchPattern.data() + 2, namePtr + 4, nameLen);
            }
            else
            {
                nameLen += 1;
                players[i].searchPattern.resize(2 + nameLen + trailBytes);
                players[i].searchPattern[2 + nameLen - 1] = '\0';
                memcpy(players[i].searchPattern.data() + 2, namePtr + 4, nameLen - 1);
            }
            memcpy(players[i].searchPattern.data(), &nameLen, 2);
            memcpy(players[i].searchPattern.data() + 2 + nameLen, _startInfoPatternTrail, trailBytes);
            _readPascalString(players[i].name, true, true);
        }

        // \note string::back(): This function shall not be called on empty strings.
        if (!players[i].name.empty() && '\0' == players[i].name.back())
        {
            players[i].name.resize(players[i].name.size() - 1);
        }
    }
}