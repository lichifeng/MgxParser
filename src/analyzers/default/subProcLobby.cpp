/**
 * \file       subProcLobby.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::_lobbyAnalyzer(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _curPos = _lobbyStartPos;

    if (save_version_ >= 13.3399)
        _skip(5);
    if (save_version_ >= 20.0599)
        _skip(9);
    if (save_version_ >= 26.1599)
        _skip(5);
    for (size_t i = 1; i < 9; i++)
    {
        if (players[i].resolvedTeamID != 255)
        {
            _skip(1);
        }
        else
        {
            _readBytes(1, &players[i].resolvedTeamID);
        }
    }
    if (save_version_ < 12.2999)
        _skip(1);
    _readBytes(4, &revealMap);
    _readBytes(4, &fogOfWar);
    _readBytes(4, &mapSize);
    if (populationLimit == UINT32_INIT)
    {
        _readBytes(4, &populationLimit);
        if (populationLimit < 40)
        {
            populationLimit *= 25;
        }
    }
    else
    {
        _skip(4);
    }
    _readBytes(1, &gameType);
    _readBytes(1, &lockDiplomacy);
    if (IS_HD(version_code_) || IS_DE(version_code_))
    {
        _readBytes(1, &treatyLength);
        _skip(4); /// \note cheat codes used, what's this for?
        if (save_version_ >= 13.1299)
            _skip(4);
        if (save_version_ >= 25.2199)
            _skip(1);
    }

    // Read lobby(pregame) talks
    if (!IS_AOK(version_code_))
    {
        Chat tmpChat;
        int32_t numChat;

        _readBytes(4, &numChat);
        if (numChat > 1000) // numChat is considered <= 50. Incase future versions set a bigger limit, use 1000 here.
        {
            logger_->warn(
                "{}(): Pregame chat count is too big. @{}.",
                __FUNCTION__, _distance());
            _sendExceptionSignal();
            numChat = 50;
        }
        while (numChat-- > 0 && _remainBytes() >= 4)
        {
            _readPascalString(tmpChat.msg, true, true);
            if (tmpChat.msg.length() > 3) // 有时候有长度为1，内容为空的。一般来说因为有“@#2”的存在，至少为3
            {
                if ('\0' == tmpChat.msg.back())
                {
                    if (tmpChat.msg.size() > 0)
                        tmpChat.msg.resize(tmpChat.msg.size() - 1);
                }

                chat.emplace_back(tmpChat);
            }
            else
            {
                if (!IS_DE(version_code_))
                    ++numChat; /// \todo Verify this.
            }
        }
    }

    if (IS_DE(version_code_) && _remainBytes() >= 4)
        _readBytes(4, &DE_mapSeed);
    // Let it go after this.
}