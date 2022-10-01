/**
 * \file       subProcessLobby.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_lobbyAnalyzer() {
    _curPos = _lobbyStartPos;

    if (saveVersion >= 13.3399) _skip(5);
    if (saveVersion >= 20.0599) _skip(9);
    if (saveVersion >= 26.1599) _skip(5);
    for (size_t i = 1; i < 9; i++)
    {
        if (players[i].resolvedTeamID != 255) {
            _skip(1);
        } else {
            _readBytes(1, &players[i].resolvedTeamID);
        }
    }
    if (saveVersion < 12.2999) _skip(1);
    _readBytes(4, &revealMap);
    _skip(4);
    _readBytes(4, &mapSize);
    _readBytes(4, &populationLimit);
    _readBytes(1, &gameType);
    _readBytes(1, &lockDiplomacy);
    if (IS_HD(versionCode) || IS_DE(versionCode)) {
        _readBytes(1, &treatyLength);
        _skip(4); /// \note cheat codes used, what's this for?
        if (saveVersion >= 13.1299) _skip(4);
        if (saveVersion >= 25.2199) _skip(1);
    }

    // Read lobby(pregame) talks
    if (!IS_AOK(versionCode)) {
        string tmpS;
        uint32_t numChat;

        _readBytes(4, &numChat);
        while (numChat-- > 0 && _remainBytes() >= 4)
        {
            _readPascalString(tmpS, true, true);
            if (tmpS.length() > 0) {
                chat.emplace_back(tmpS);
            } else {
                ++numChat;
            }
        }
    }
    
    if (IS_DE(versionCode) && _remainBytes() >= 4) _readBytes(4, &mapSeed);
    // Let it go after this.
}