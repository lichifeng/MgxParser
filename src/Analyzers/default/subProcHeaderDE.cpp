/**
 * \file       subProcHeaderDE.cpp
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

void DefaultAnalyzer::_headerDEAnalyzer() {
    if (saveVersion >= 25.2199) _readBytes(4, &DE_build);
    if (saveVersion >= 26.1599) _readBytes(4, &DE_timestamp);
    _readBytes(4, &DD_version);
    _readBytes(4, &DD_internalVersion);
    _readBytes(4, &DD_gameOptionsVersion);
    _readBytes(4, &DD_DLCCount);
    _skip(DD_DLCCount * 4);
    _readBytes(4, &DD_datasetRef);
    _readBytes(4, &DD_difficultyID);
    _readBytes(4, &DD_selectedMapID);
    _readBytes(4, &DD_resolvedMapID);
    _readBytes(4, &revealMap);
    _readBytes(4, &DD_victoryTypeID);
    _readBytes(4, &DD_startingResourcesID);
    _readBytes(4, &DD_startingAgeID);
    _readBytes(4, &DD_endingAgeID);
    _readBytes(4, &DD_gameType);
    _expectBytes(
        patterns::HDseparator, // a3 5f 02 00
        "[INFO] Analyzing DE-specific data section in header stream. \n", 
        "[WARN] Unexpected validating pattern DE-specific data section in header stream. \n",
        false
    );
    _skip(8); // 2 separators
    _readBytes(4, &DD_speed);
    _readBytes(4, &DD_treatyLength);
    _readBytes(4, &DD_populationLimit);
    _readBytes(4, &DD_numPlayers);
    _readBytes(4, &DD_unusedPlayerColor);
    _readBytes(4, &DD_victoryAmount);
    // Next 4 bytes should be: a3 5f 02 00
    _skip(4);
    _readBytes(1, &DD_tradeEnabled);
    _readBytes(1, &DD_teamBonusDisabled);
    _readBytes(1, &DD_randomPositions);
    _readBytes(1, &DD_allTechs);
    _readBytes(1, &DD_numStartingUnits);
    _readBytes(1, &DD_lockTeams);
    _readBytes(1, &DD_lockSpeed);
    _readBytes(1, &DD_multiplayer);
    _readBytes(1, &DD_cheats);
    _readBytes(1, &DD_recordGame);
    _readBytes(1, &DD_animalsEnabled);
    _readBytes(1, &DD_predatorsEnabled);
    _readBytes(1, &DD_turboEnabled);
    _readBytes(1, &DD_sharedExploration);
    _readBytes(1, &DD_teamPositions);
    if (saveVersion >= 13.3399) _readBytes(4, &DD_subGameMode);
    if (saveVersion >= 13.3399) _readBytes(4, &DD_battleRoyaleTime);
    if (saveVersion >= 25.0599) _readBytes(1, &DD_handicap);
    // Next 4 bytes Should be: a3 5f 02 00
    _expectBytes(
        patterns::HDseparator,
        "[INFO] Ready to parse DE players data, everything ok until now. \n", 
        "[WARN] Unexpected validating pattern in DE-specific data section in header stream before players data part. \n"
    );

    // Read player data
    for (size_t i = 1; i < 9; i++)
    {
        _readBytes(4, &players[i].DD_DLCID);
        _readBytes(4, &players[i].colorID);
        _readBytes(1, &players[i].DE_selectedColor);
        _readBytes(1, &players[i].DE_selectedTeamID);
        _readBytes(1, &players[i].resolvedTeamID);
        players[i].DE_datCrc = hexStr(_curPos, 8, true);
        _readBytes(1, &players[i].DD_MPGameVersion);
        _readBytes(4, &players[i].civID);
        _readDEString(players[i].DD_AIType);
        _readBytes(1, &players[i].DD_AICivNameIndex);
        _readDEString(players[i].DD_AIName);
        _readDEString(players[i].name);
        _readBytes(4, &players[i].type);
        if (players[i].type == 4) ++_DD_AICount;
        _readBytes(4, &players[i].DE_profileID);
        _skip(4); // Should be: 00 00 00 00
        _readBytes(4, &players[i].DD_playerNumber); /// \note 不存在的话是 -1 FF FF FF FF
        if (saveVersion < 25.2199) _readBytes(4, &players[i].DD_RMRating);
        if (saveVersion < 25.2199) _readBytes(4, &players[i].DD_DMRating);
        _readBytes(1, &players[i].DE_preferRandom);
        _readBytes(1, &players[i].DE_customAI);
        //cout << players[i].DD_RMRating << "  " << players[i].DD_DMRating << endl;
        if (saveVersion >= 25.0599) _readBytes(8, players[i].DE_handicap); /// \todo what's this nonsense?
    }
    _skip(9);
    _readBytes(1, &DD_fogOfWar);
    _readBytes(1, &DD_cheatNotifications);
    _readBytes(1, &DD_coloredChat);
    _skip(4); // 0xa3, 0x5f, 0x02, 0x00
    _readBytes(1, &DD_isRanked);
    _readBytes(1, &DD_allowSpecs);
    _readBytes(4, &DD_lobbyVisibility);
    _readBytes(1, &DE_hiddenCivs);
    _readBytes(1, &DE_matchMaking);
    _readBytes(4, &DE_specDely);
    if (saveVersion >= 13.1299) _readBytes(1, &DE_scenarioCiv);
    if (saveVersion >= 13.1299) DE_RMSCrc = hexStr(_curPos, 4, true);
    
    /// \warning 实话我也不知道这一段是什么鬼东西，只好用搜索乱撞过去了.下面是做的一些研究，找的规律
    /// \note // de-13.03.aoe2record : 2a 00 00 00 fe ff ff ff + 59*(fe ff ff ff)
    /// de-13.06.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.07.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(fe ff ff ff)
    /// de-13.08.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(fe ff ff ff)
    /// de-13.13.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.15.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.17.aoe2record : 2C 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.20.aoe2record : 2C 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.34.aoe2record : 2D 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-20.06.aoe2record : 2D 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-20.16.aoe2record : 2D 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.01.aoe2record : 2E 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.02.aoe2record : 2E 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.06.aoe2record : 2F 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.22.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    /// de-26.16.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    /// de-26.18.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    /// de-26.21.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    auto curItr = _header.begin() + (_curPos - _curStream);
    for (size_t i = 0; i < 23; i++)
    {
        curItr = findPosition(
            curItr, _header.end(),
            patterns::HDStringSeparator.begin(), 
            patterns::HDStringSeparator.end()
        );
        if (curItr != _header.end()) {
            curItr += (4 + *((uint16_t*)(&(*curItr) + 2)));
            _curPos = &(*curItr);
        }
    }
    _skip(4); /// \note 2a/c/d/e/f 00 00 00
    if (saveVersion >= 25.2199)
    {
        if (*(uint32_t*)_curPos != 0)
            throw(ParserException("[ALERT] expecting 00 00 00 00 but disppointed. \n"));
        _skip(4);
    } else {
        if (*(uint32_t*)_curPos != -2)
            throw(ParserException("[ALERT] expecting FE FF FF FF but disppointed. \n"));
        _skip(60 * 4);
    }
    _readBytes(8, &DE_numAIFiles);
    if (DE_numAIFiles > 1000)
        message.append("[ALERT] Is DE_numAIFiles too big?? \n");
    for (size_t i = 0; i < DE_numAIFiles; i++)
    {
        _skip(4);
        _skipDEString(); /// \todo AI FILENAMES deserve to be recorded.
        _skip(4);
    }
    if (saveVersion >= 25.0199) _skip(8);
    DD_guid = hexStr(_curPos, 16, true);
    message.append("[INFO] Reached GUID milestone: " + DD_guid + " \n");
    _readDEString(DD_lobbyName);
    if (saveVersion >= 25.2199) _skip(8);
    _readDEString(DD_moddedDataset);
    _skip(19);
    if (saveVersion >= 13.1299) _skip(5);
    if (saveVersion >= 13.1699) _skip(9);
    if (saveVersion >= 20.0599) _skip(1);
    if (saveVersion >= 20.1599) _skip(8);
    if (saveVersion >= 25.0599) _skip(21);
    if (saveVersion >= 25.2199) _skip(4);
    if (saveVersion >= 26.1599) _skip(8);
    _skipDEString();
    _skip(5);
    if (saveVersion >= 13.1299) _skip(1);
    if (saveVersion < 13.1699) {
        _skipDEString();
        _skip(8); /// uint32 + 00 00 00 00
    }
    if (saveVersion >= 13.1699) _skip(2);
}