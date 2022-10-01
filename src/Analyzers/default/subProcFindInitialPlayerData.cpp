/**
 * \file       subProcessFindInitialPlayerData.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"
#include "utils.h"

void DefaultAnalyzer::_findInitialPlayersDataPos() {
    _curPos = _startInfoPos + 2 + numPlayers + 36 + 4 + 1;

    uint32_t easySkip = easySkipBase + mapCoord[0] * mapCoord[1];
    auto itStart = _header.cbegin() + (_curPos - _header.data()) + easySkip;
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
            players[i].searchPattern.cend()
        );
        if (found == itEnd) {
            cout << "now at: " << i << " " << players[i].name << endl;;
            throw(AnalyzerException("[WARN] Cannot find satisfied player data in startinfo. \n"));
        }
        players[i].dataOffset = found - _header.cbegin() - (2 + numPlayers + 36 + 4 + 1);
        itStart = found + easySkip;
    }
    message.append("[INFO] Located player initial data position in start info section. \n");
}