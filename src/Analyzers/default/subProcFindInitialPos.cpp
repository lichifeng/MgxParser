/**
 * \file       subProcFindInitialPos.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"
#include "utils.h"

void DefaultAnalyzer::_findInitialPlayersDataPos(int debugFlag)
{
    _debugFlag = debugFlag;

    if (_startInfoPos)
    {
        _curPos = _startInfoPos + 2 + numPlayers + 36 + 4 + 1;
    }
    else
    {
        _curPos = _header.data();
    }

    uint32_t easySkip = easySkipBase + mapCoord[0] * mapCoord[1];
    auto itStart = _header.cbegin() + (_curPos - _header.data()) + easySkip;

    if (nullptr == _scenarioHeaderPos)
        _scenarioHeaderPos = _victoryStartPos ? _victoryStartPos
                                              : (_disablesStartPos ? _disablesStartPos : _gameSettingsPos);
    auto itEnd = _header.cbegin() + (_scenarioHeaderPos - _header.data() - numPlayers * 1817); // Achievement section is 1817 * numPlayers bytes

    // Length of every player's data is at least easySkip bytes (and ususally much
    // more), we can use this to escape unnecessary search
    // First player don't need a search (and cannot, 'cuz different behavior
    // among versions)
    players[0].dataOffset = _curPos - _header.data();

    auto found = itStart;
    bool indexFound[9] = {false};
    for (size_t i = 1; i < 9; i++)
    {
        if (!players[i].valid() || players[i].index < 0 || players[i].index > 8 || indexFound[players[i].index])
            continue;

        found = findPosition(
            itStart, itEnd,
            players[i].searchPattern.cbegin(),
            players[i].searchPattern.cend());
        if (found == itEnd)
        {
            // \todo 如果只是当前这个没有找到，那是不是应该退回查找起始位置呢？
            // 多控的情况下只有一个玩家会有这里的信息，他们的顺序难道是固定的？
            found = findPosition(
                itStart, itEnd,
                players[i].searchPattern.cend() - trailBytes,
                players[i].searchPattern.cend());

            if (found != itEnd)
            {
                _curPos = &found[0] - 3;
                // 倒着往前找字符串，一般字符串最后一个字节是'\0'，表示长度又有两个字节，所以往前退3个字节开始查找
                for (size_t j = 3; j < 300; ++j)
                {
                    if (2 + *(uint16_t *)_curPos == j)
                    {
                        players[i].dataOffset = _curPos - _curStream->data();

                        if ('\0' == *(_curPos + j - 1))
                            players[i].name.assign((char *)(_curPos + 2));
                        else
                            _readPascalString(players[i].name);

                        break;
                    }
                    --_curPos;
                }
            }
        }
        else
        {
            players[i].dataOffset = found - _header.cbegin();
        }

        if (0 != players[i].dataOffset)
        {
            indexFound[players[i].index] = true;
            itStart = found + easySkip;
        }
        else
        {
            logger_->warn(
                "{}(): Cannot find data of player[{}]: {} Type:{} in startinfo. @{}.",
                __FUNCTION__, i, players[i].name, players[i].type, _distance());
            _sendExceptionSignal();
            continue;
        }
    }
}