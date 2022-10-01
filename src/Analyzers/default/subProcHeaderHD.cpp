/**
 * \file       subProcHeaderHD.cpp
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

void DefaultAnalyzer::_headerHDAnalyzer() {
    int16_t  tmpInt16;
    uint8_t* tmpPos;
    _readBytes(4, &DD_version);
    if (DD_version - 1006 < 0.0001) versionCode = HD57;
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
    if (DD_version >= 1005.9999) _readBytes(4, &DD_gameType);
    _expectBytes(
        patterns::HDseparator,
        "[INFO] Analyzing HD-specific data section in header stream. \n", 
        "[WARN] Unexpected validating pattern HD-specific data section in header stream. \n"
    );
    if (DD_version == 1000) {
        _readBytes(2, &tmpInt16);
        // Next 2 bytes should be: [ 60 0A ]
        _skip(2);
        _readBytes(tmpInt16, HD_ver1000MapName.data());

        _skipHDString();
    }
    // Next 4 bytes should be: a3 5f 02 00
    _skip(4);
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
    /// \todo Next 4 bytes should be: a3 5f 02 00, but aoc-mgz indicates they are following data
    // _readBytes(1, &HD_turboEnabled);
    // _readBytes(1, &HD_sharedExploration);
    // _readBytes(1, &HD_teamPositions);
    // _skip(1); // Unknown byte
    _skip(4);
    if (DD_version == 1000) {
        _skip(120); // 40 * 3
        _skip(4); // a3 5f 02 00
        _skip(40);
        for (size_t i = 0; i < 8; i++)
        {
            _skipHDString();
        }
        _skip(16);
        _skip(4); // a3 5f 02 00
        _skip(10);
    } else {
        // Check if HD version is between [5.0, 5.7]
        int32_t check, test;
        tmpPos = _curPos;
        _readBytes(4, &check);
        _skip(4);
        if (DD_version >= 1005.9999) _skip(1);
        _skip(15);
        _skipHDString();
        _skip(1);
        if (DD_version >= 1004.9999) _skipHDString();
        _skipHDString();
        _skip(16);
        _readBytes(4, &test);
        if (check != test) versionCode = HD58;
        _curPos = tmpPos;

        // Read player data
        for (size_t i = 1; i < 9; i++)
        {
            _readBytes(4, &players[i].DD_DLCID);
            _readBytes(4, &players[i].colorID);
            if (DD_version >= 1005.9999) _skip(1);
            _skip(2);
            _readBytes(4, &players[i].HD_datCrc);
            _readBytes(1, &players[i].DD_MPGameVersion);
            //_readBytes(4, &players[i].HD_teamIndex); // Not correct team index
            _skip(4);
            _readBytes(4, &players[i].civID);
            _readHDString(players[i].DD_AIType);
            _readBytes(1, &players[i].DD_AICivNameIndex);
            if (DD_version >=1004.9999) _readHDString(players[i].DD_AIName);
            _readHDString(players[i].name);
            _readBytes(4, &players[i].type);
            _readBytes(8, &players[i].HD_steamID);
            _readBytes(4, &players[i].DD_playerNumber);
            if (DD_version >= 1005.9999 
                && versionCode != HD50_6 
                && versionCode != HD57)
            {
                _readBytes(4, &players[i].DD_RMRating);
                _readBytes(4, &players[i].DD_DMRating);
            }
        }
        
        _readBytes(1, &DD_fogOfWar);
        _readBytes(1, &DD_cheatNotifications);
        _readBytes(1, &DD_coloredChat);
        _skip(13); /// 9 bytes + a3 5f 02 00
        _readBytes(1, &DD_isRanked);
        _readBytes(1, &DD_allowSpecs);
        _readBytes(4, &DD_lobbyVisibility);
        _readBytes(4, &HD_customRandomMapFileCrc);
        _readHDString(HD_customScenarioOrCampaignFile);
        _skip(8);
        _readHDString(HD_customRandomMapFile);
        _skip(8);
        _readHDString(HD_customRandomMapScenarionFile);
        _skip(8);
        DD_guid = hexStr(_curPos, 16, true); /// \todo should this map to gamehash or filehash?
        _readHDString(DD_lobbyName);
        _readHDString(DD_moddedDataset);
        HD_moddedDatasetWorkshopID = hexStr(_curPos, 4, true);
        if (DD_version >= 1004.9999)
        {
            _skip(4);
            _skipHDString();
            _skip(4);
        }
        
    }
}