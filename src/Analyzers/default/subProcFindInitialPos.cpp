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

#include "Analyzer.h"
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
    for (size_t i = 1; i < numPlayers; i++)
    {
        found = findPosition(
            itStart, itEnd,
            players[i].searchPattern.cbegin(),
            players[i].searchPattern.cend());
        if (found == itEnd)
        {
            // \todo 如果只是当前这个没有找到，那是不是应该退回查找起始位置呢？多控的情况下只有一个玩家会有这里的信息，他们的顺序难道是固定的？
            logger->warn(
                "{}(): Cannot find player[{}]'s data in startinfo. @{}.",
                __FUNCTION__, i, _distance());
            _sendFailedSignal();
            return;
        }
        players[i].dataOffset = found - _header.cbegin() - (2 + numPlayers + 36 + 4 + 1);
        itStart = found + easySkip;
    }
}