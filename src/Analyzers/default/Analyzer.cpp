/**
 * \file       Analyzer.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-22
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#define ZLIB_CHUNK 8192

#include <filesystem>
#include <sstream>
#include "zlib.h"
#include "Analyzer.h"
#include "utils.h"
#include "TileStructures.h"

using namespace std;

void DefaultAnalyzer::run() {
    auto p = filesystem::path(path);

    if (!filesystem::exists(path))
    {
        status = "fail";
        message.append(path + " does not exists. \n");
        throw(AnalyzerException(message));
    }

    filename = p.filename();
    ext = p.extension();
    filesize = filesystem::file_size(p);

    // Try open record file
    _f.open(path, ifstream::in | ifstream::binary);
    if (!_f.is_open())
    {
        status = "fail";
        message.append("Failed to open " + path + ". \n");
        throw(AnalyzerException(message));
    }
    
    // Try to extract header&body streams
    if (!_locateStreams())
    {
        status = "fail";
        message.append("Failed to unzip raw header data. \n");
        throw(AnalyzerException(message));
    }

    _f.close();

    // Log current stage
    status = "good";
    message.append("[INFO] Successfully extracted header&body streams from this file. Ready for data analyzing. \n");
    
    // Start data analyzing
    _analyze();
}

bool DefaultAnalyzer::_locateStreams() {
    /**
     * \brief      headerMeta[0]: header_len + next_pos + header_data (Data
     * length) \n headerMeta[1]: nextPos
     *
     */
    int32_t headerMeta[2];
    _f.read((char*)headerMeta, 8);

    // Construct body stream
    _bodySize = filesize - headerMeta[0];
    _body.resize(_bodySize); /// \note vector 的 resize 和 reserve 是不一样的，这里因为这个问题卡了很久。reserve 并不会初始化空间，因此也不能直接读数据进去。
    _f.seekg(headerMeta[0]);
    _f.read((char*)_body.data(), _bodySize);

    uintmax_t rawHeaderPos = headerMeta[1] < filesize ? 8 : 4;
    _f.seekg(rawHeaderPos);
    if (rawHeaderPos == 4) versionCode = AOK;

    // Try to unzip header data
    if (_inflateRawHeader() != 0) return false;

    return true;
}

void DefaultAnalyzer::_analyze() {
    // Start data analyzing
    
    // Get logVersion
    _switchStream(BODY_STRM);
    _readBytes(4, &logVersion);
    _switchStream(HEADER_STRM);

    _readBytes(8, versionStr);
    _readBytes(4, &saveVersion);
    _setVersionCode();

    // Analyze HD/DE-specific data in header data
    if (IS_HD(versionCode) && saveVersion > 12.3401) {
        _headerHDAnalyzer();
    }

    if (IS_DE(versionCode)) {
        _headerDEAnalyzer();
    }
    
    // Analyze AI info part in header data
    _AIAnalyzer();

    // Analyze replay part in header data
    _replayAnalyzer();

    // Analyze map data in header stream
    _mapDataAnalyzer();

    // Analyze start info
    _findStartInfoStart();

    // Find some key positions first
    _findTriggerInfoStart();
    _findGameSettingsStart();
    _findDisablesStart();
    _findVictoryStart();
    _findScenarioHeaderStart();

    // Skip scenario header. Nothing valuable here except a filename.
    // What is really needed is instructions data after this section,
    // Which is critical data to detect file encoding.
    _scenarioHeaderAnalyzer();

    // Read messages
    _messagesAnalyzer();

    // Read victory-related settings
    _victorySettingsAnalyzer();

    // Analyze game settings part, player names first appears here (before HD/DE versions).
    _gameSettingsAnalyzer();

    // Search initial player data postion
    _searchInitialPlayersDataPos();

    // Go back to player initial data position and rummage some useful pieces
    _startInfoAnalyzer();

    // Trigger info is normally skipped
    _triggerInfoAnalyzer();

    // Analyze some misc data
    _lobbyAnalyzer();
}

void DefaultAnalyzer::_lobbyAnalyzer() {
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

/**
 * \brief      This method is not fully tested
 * 
 */
void DefaultAnalyzer::_triggerInfoAnalyzer() {
    _curPos = _triggerInfoPos;

    int conditionSize = 72;
    int32_t
        numTriggers,
        descriptionLen,
        nameLen,
        numEffects,
        numSelectedObjs,
        textLen,
        soundFilenameLen,
        numConditions;
    bool isHDPatch4 = saveVersion >= 12.3399;
    
    _skip(1);

    if (isHDPatch4) conditionSize += 8;

    _readBytes(4, &numTriggers);
    for (int i = 0; i < numTriggers; i++)
    {
        _skip(18);
        _readBytes(4, &descriptionLen);
        _skip(descriptionLen > 0 ? descriptionLen : 0);
        _readBytes(4, &nameLen);
        _skip(nameLen > 0 ? nameLen : 0);
        _readBytes(4, &numEffects);
        for (int i = 0; i < numEffects; i++)
        {
            _skip(24);
            _readBytes(4, &numSelectedObjs);
            numSelectedObjs = numSelectedObjs == -1 ? 0 : numSelectedObjs;
            _skip(isHDPatch4 ? 76 : 72);
            _readBytes(4, &textLen);
            _skip(textLen > 0 ? textLen : 0);
            _readBytes(4, &soundFilenameLen);
            _skip(soundFilenameLen > 0 ? soundFilenameLen : 0);
            _skip(numSelectedObjs * 4);
        }
        _skip(numEffects * 4);
        _readBytes(4, &numConditions);
        _skip(numConditions * conditionSize + numConditions * 4);
    }

    if (numTriggers > 0) _skip(numTriggers * 4);

    if (IS_DE(versionCode)) _skip(1032);
}

void DefaultAnalyzer::_startInfoAnalyzer() {
    uint32_t numHeaderData = *(uint32_t*)(_startInfoPatternTrail + 1);

    for (auto p : players) {
        if (p.initialDataFound() && p.valid()) {
            _curPos = _header.data() + p.dataOffset;
            _skip(2 + numPlayers + 36 + 4 + 1);
            _skipPascalString();
            _skip(762);
            if (saveVersion >= 11.7599) _skip(36);
            if (IS_DE(versionCode) || IS_HD(versionCode)) _skip(4 * (numHeaderData - 198));
            if (versionCode == USERPATCH15 || versionCode == MCP) {
                _readBytes(4, &p.modVersionID);
                _skip(4 * 7 + 4 * 28);
            }
            if (versionCode == MCP) _skip(4 * 65);
            _skip(1);
            _readBytes(8, p.initCamera);

            // Do some check here
            if (
                   p.initCamera[0] < 0 
                || p.initCamera[0] > mapCoord[0] 
                || p.initCamera[1] < 0
                || p.initCamera[1] > mapCoord[1]
            ) {
                message.append("[WARN] Bad init camera was found. \n");
                p.initCamera[0] = p.initCamera[1] = -1.0;
                continue;
            }


            if (!IS_AOK(versionCode)) _skip(*(float*)_curPos * 4);
            _skip(4 + 1);
            _readBytes(1, &p.civID);
            _skip(3);
            _readBytes(1, &p.colorID);
        }
    }
}

void DefaultAnalyzer::_searchInitialPlayersDataPos() {
    _curPos = _startInfoPos + 2 + numPlayers + 36 + 4 + 1;

    uint32_t easySkip = easySkipBase + mapCoord[0] * mapCoord[1];
    auto itStart = _header.cbegin() + (_curPos - _header.data()) + easySkip;
    auto itEnd = _header.cbegin() + (_scenarioHeaderPos - _header.data() - numPlayers * 1817); // Achievement section is 1817 * numPlayers bytes

    // Length of every player's data is at least 18000 bytes (and ususally much
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

/**
 * \brief      This method also builds some search patterns used to traverse in startinfo section
 * 
 */
void DefaultAnalyzer::_gameSettingsAnalyzer() {
    _curPos = _gameSettingsPos;
    _skip(64 + 4 + 8);
    if (IS_HD(versionCode)) _skip(16);
    if (!IS_AOK(versionCode)) _readBytes(4, &mapID);
    _readBytes(4, &difficultyID);
    _readBytes(4, &lockTeams);
    if (IS_DE(versionCode)) {
        _skip(29);
        if (saveVersion >= 13.0699) _skip(1);
        if (saveVersion >= 13.3399) _skip(132);
        if (saveVersion >= 20.0599) _skip(1);
        if (saveVersion >= 20.1599) _skip(4);
        if (saveVersion >= 25.0199) _skip(4 * 16);
        if (saveVersion >= 25.0599 && saveVersion < 26.2099) _skip(4);
    }

    uint16_t nameLen;
    uint8_t* namePtr;
    for (size_t i = 0; i < 9; i++)
    {
        namePtr = _curPos + 8;
        // 先获得名字长度，再把 len + lenName + trailBytes 放到 pattern 里，
        // 供以后搜索使用， 然后再跳过或把名字转码保存下来。
        // trailBytes 只保存了一个指针，具体用后面的几个字节组装可以试下，看
        // 看性能表现，一般建议在[1, 6]个，第一个一般是0x16,第六个一般是
        // 0x21，中间一个4字节的数字则根据版本不同有所区别。
        // 要注意的是，HD/DE版本中，startinfo里的名字数据和HD/DE header里的一
        // 样，和这里不一样。这里显示的玩家名是“玩家1”之类。
        // 还有一个要注意的是，startinfo中字符串长度用2字节表示，这里用4字节。
        // \todo 这里似乎只有玩家才会在HD/DE专有区块有名字信息，GAIA和AI没有。

        _readBytes(4, &players[i].slot);
        if (players[i].name.length() > 0) {
            // Data from HD/DE header has higher priority 
            _skip(4);
            nameLen = players[i].name.length() + 1;
            players[i].searchPattern.resize(2 + nameLen + trailBytes);
            memcpy(players[i].searchPattern.data(), &nameLen, 2);
            memcpy(players[i].searchPattern.data() + 2, players[i].name.data(), nameLen);
            memcpy(players[i].searchPattern.data() + 2 + nameLen, _startInfoPatternTrail, trailBytes);
            _skipPascalString(true);
        } else {
            _readBytes(4, &players[i].type);
            // 两处字符串前面有\0，后面有的版本有，有的版本没有，需要判断处理
            nameLen = *(uint32_t*)namePtr;
            if ('\0' == *(namePtr + 4 + nameLen - 1)) {
                players[i].searchPattern.resize(2 + nameLen + trailBytes);
                memcpy(players[i].searchPattern.data() + 2, namePtr + 4, nameLen);
            } else {
                nameLen += 1;
                players[i].searchPattern.resize(2 + nameLen + trailBytes);
                players[i].searchPattern[2 + nameLen - 1] = '\0';
                memcpy(players[i].searchPattern.data() + 2, namePtr + 4, nameLen - 1);
            }
            memcpy(players[i].searchPattern.data(), &nameLen, 2);
            memcpy(players[i].searchPattern.data() + 2 + nameLen, _startInfoPatternTrail, trailBytes);
            _readPascalString(players[i].name, true, true);
        }
    }

}

void DefaultAnalyzer::_victorySettingsAnalyzer() {
    _curPos = _victoryStartPos;
    _skip(4);
    _readBytes(4, &victoryIsConquest);
    _skip(4);
    _readBytes(4, &victoryRelics);
    _skip(4);
    _readBytes(4, &victoryExplored);
    _skip(4);
    _readBytes(4, &victoryAnyOrAll);
    _readBytes(4, &victoryMode);
    _readBytes(4, &victoryScore);
    _readBytes(4, &victoryTime);
    
    // Do a simple check with Unknown section
    if (*(uint32_t*)_curPos != 3) {
        message.append("[WARN] Check bytes following victory section seems wrong, expected 03 00 00 00.");
    }
}

void DefaultAnalyzer::_messagesAnalyzer() {
    _curPos = _messagesStartPos;
    _skip(20);
    if (!IS_AOK(versionCode)) _skip(4);

    _readPascalString(instructions);
    _guessEncoding();

    int totalStrs = IS_AOK(versionCode) ? 8 : 9; /// \todo 需要验证 AOK 的情况
    for (size_t i = 0; i < totalStrs; i++)
        _skipPascalString();
    
    /// \note 这一节的数据剩下的就不读了，没什么用。以后再说吧。

}

void DefaultAnalyzer::_scenarioHeaderAnalyzer() {
    uint16_t filenameLen;
    _curPos = _scenarioHeaderPos;
    _skip(4433);
    _readBytes(2, &filenameLen);
    if (filenameLen > 224) throw(AnalyzerException("[WARN] scenarioFilename is unsually long. \n"));
    scenarioFilename.assign((char*)(_curPos+2), filenameLen);
    _skip(filenameLen);
    if (IS_DE(versionCode)) {
        _skip(64);
        if (saveVersion >= 13.3399) {
            _skip(64);
        }
    }
    _messagesStartPos = _curPos;
}

void DefaultAnalyzer::_findScenarioHeaderStart() {
    // Try to locate scenario version data in DE (this float in de varies
    // among minor game versions and relative stable in previous versions.)
    if (IS_DE(versionCode)) {
        size_t searchSpan = 8000; // usually 5500~6500
        auto startPoint = _victoryStartPos - searchSpan;
        _curPos = startPoint;
        for (size_t i = 0; i < searchSpan; i++)
        {
            scenarioVersion = *(float*)startPoint;
            if (scenarioVersion > 1.35 && scenarioVersion < 1.55) {
                _scenarioHeaderPos = _curPos = startPoint - 4; // 4 bytes are 00 00 00 00 before scenario version
                message.append("[INFO] Reach _scenarioHeaderPos by scenario version range test. \n");
                return;
            }
            ++startPoint;
        }
        throw(AnalyzerException("[WARN] Cannot find satisfied _scenarioHeaderPos in this DE version. \n"));
    }
    

    auto scenarioSeprator = IS_AOK(versionCode) ? \
        patterns::scenarioConstantAOK : \
        (
            IS_HD(versionCode) ? 
                (
                    /// \todo 这里应该是13.3399还是13.3599?
                    saveVersion >= 12.3599 ? \
                    patterns::scenarioConstantHD : \
                    patterns::scenarioConstantMGX2
                ) :
            patterns::scenarioConstantAOC
        );
        
    vector<uint8_t>::reverse_iterator rFound;
    rFound = findPosition(
        make_reverse_iterator(_header.begin() + (_victoryStartPos - _header.data())),
        _header.rend(),
        scenarioSeprator.rbegin(),
        scenarioSeprator.rend()
    );
    if (rFound == _header.rend()) {
        /// \todo aoc-mgz says aok scenario version can appear in UP
        throw(AnalyzerException("[WARN] Cannot find satisfied _scenarioHeaderPos. \n"));
    } else {
        _scenarioHeaderPos = _curPos = &*(--rFound) - 4 - scenarioSeprator.size();
        message.append("[INFO] Reach _scenarioHeaderPos and passed validation. \n");
    }
}

void DefaultAnalyzer::_findVictoryStart() {
    _victoryStartPos = _disablesStartPos - 12544 - 44;
    // Check if at correct point
    if (*(int32_t*)_victoryStartPos != -99) {
        throw(AnalyzerException("[WARN] Check bytes not valid, _victoryStartPos seems not good. \n")); // 9d ff ff ff
    } else {
        message.append("[INFO] Reach _victoryStartPos and passed validation. \n");
    }
}

void DefaultAnalyzer::_findGameSettingsStart() {
    vector<uint8_t>::reverse_iterator rFound;
    rFound = findPosition(
        make_reverse_iterator(_header.begin() + (_triggerInfoPos - _header.data())),
        _header.rend(), 
        patterns::separator.rbegin(),
        patterns::separator.rend()
    );
    if (rFound == _header.rend()) {
        throw(AnalyzerException("[WARN] Cannot find satisfied _gameSettingsPos. \n"));
    } else {
        _curPos = &(*--rFound) - 68;
    }
    if ((*(int32_t*)(_curPos + 68) != -1) || (*(int32_t*)(_curPos + 72) != -1)) {
        throw(AnalyzerException("[WARN] Abnormal gamesettings start position. \n"));
    } else {
        message.append("[INFO] Found gamesettings start position (before starting age data). \n");
        _gameSettingsPos = _curPos;
    }
}

void DefaultAnalyzer::_findDisablesStart() {
    _curPos = _gameSettingsPos;
    if (!IS_DE(versionCode)) {
        if (IS_HD(versionCode) && saveVersion < 12.3399) {
            _disablesStartPos = _curPos - 5396;
        } else {
            _disablesStartPos = _curPos - 5392;
        }
    } else {
        _disablesStartPos = _curPos - 276;
    }
    if (IS_HD(versionCode) && saveVersion >= 12.3399) {
        _disablesStartPos -= 644;
    }
    
    // Check if at correct point
    if (*(int32_t*)_disablesStartPos != -99) {
        _curPos -= 4; /// 先后退四个字节再开始查找
        for (size_t i = 0; i < 8000; i++) /// \todo 这个数随便定的，.mgx2文件里好像差几个字节，不管了，反正特殊情况就查找吧
        {
            if (*(int32_t*)_curPos == -99)
            {
                _disablesStartPos = _curPos;
                break;
            }
            --_curPos;
        }
        if (*(int32_t*)_disablesStartPos != -99)
        {
            throw(AnalyzerException("[WARN] Check bytes not valid, _disablesStartPos seems not good. \n")); // 9d ff ff ff
        } else {
            message.append("[INFO] Reach _disablesStartPos by reverse search. \n");
        }
    } else {
        message.append("[INFO] Reach _disablesStartPos and passed validation. \n");
    }
}

void DefaultAnalyzer::_findTriggerInfoStart() {

    // aok ~ hd: 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, f9, 3f (double 1.6)
    // de < 13.34: 00 e0 ab 45 + double 2.2
    // de >= 13.34:  \note Maybe the cutoff point is not 13.34
    //     13.34: 00 e0 ab 45 + padding(1) + double 2.4
    //     20.06: 00 e0 ab 45 + padding(1) + double 2.4
    //     20.16: 00 e0 ab 45 + padding(11) + double 2.4
    //     25.01: 00 e0 ab 45 + padding(11) + double 2.4
    //     25.02: 00 e0 ab 45 + padding(11) + double 2.4
    //     25.06: 00 e0 ab 45 + padding(11) + double 2.5
    //     25.22: 00 e0 ab 45 + padding(11) + double 2.6
    //     26.16: 00 e0 ab 45 + padding(11) + double 3.0
    //     26.18: 00 e0 ab 45 + padding(11) + double 3.0
    //     26.18: 00 e0 ab 45 + padding(11) + double 3.2

    vector<uint8_t>::reverse_iterator rFound;

    _curPos = _startInfoPos;

    if (IS_AOK(versionCode)) {
        rFound = findPosition(
            _header.rbegin(),
            _header.rend(), 
            patterns::gameSettingSign1.rbegin(),
            patterns::gameSettingSign1.rend()
        );
        if (rFound == _header.rend()) {
            throw(AnalyzerException("[WARN] Failed to find _triggerInfoPos (No sign found). \n"));
        }
        _triggerInfoPos = _curPos = &(*--rFound);
    } else {
        rFound = findPosition(
            _header.rbegin(),
            _header.rend(), 
            patterns::gameSettingSign.rbegin(),
            patterns::gameSettingSign.rend()
        );
        if (rFound == _header.rend()) {
            throw(AnalyzerException("[WARN] Failed to find _triggerInfoPos (No sign found). \n"));
        }
        _curPos = &(*--rFound);
        double signNum = 0.0;
        for (size_t i = 0; i <= triggerStartSearchRange; i++)
        {
            signNum = *(double*)_curPos;
            if (signNum >= 1.5 && signNum <= 10) {
                _triggerInfoPos = _curPos += 8;
                return;
            } else {
                ++_curPos;
            }
        }
        throw(AnalyzerException("[WARN] Failed to find _triggerInfoPos. \n"));
    }

    /// \todo Maybe I will deploy a brutal search for double float in [1.6, 10]
    /// if cannot find it easily?
}

void DefaultAnalyzer::_findStartInfoStart() {
    _readBytes(4, &restoreTime);
    
    uint32_t numParticles;
    _readBytes(4, &numParticles);
    _skip(numParticles * 27);

    // A checkpoint, expecting 10060 with AOK and 40600 with higher version
    if (*(uint32_t*)_curPos == 40600) {
        message.append("[INFO] StartInfo section check passed, found value 40600. \n");
    } else if ((*(uint32_t*)_curPos == 10060) && IS_AOK(versionCode)) {
        message.append("[INFO] StartInfo section check passed, found value 10060. \n");
    } else {
        throw(AnalyzerException("[WARN] Cannot find expected check value 10060/40600 in start info section. \n"));
    }
    
    // Pin start info position
    _startInfoPos = _curPos += 4;

    // Fetch search pattern trail from first player (normally GAIA), laterly
    // used in gamesettings section.
    _curPos += 2 + numPlayers + 36 + 4 + 1;
    _skipPascalString();
    _startInfoPatternTrail = _curPos;
}

void DefaultAnalyzer::_mapDataAnalyzer() {
    _readBytes(8, &mapCoord);
    if (mapCoord[0] >= 10000 || mapCoord[1] >= 10000)
    {
        throw(AnalyzerException("[WARN] Abnormal map size data. \n"));
    } else if (mapCoord[0] == mapCoord[1]) {
        message.append("[INFO] Found desired map coordinates data. \n");
    } else {
        throw(AnalyzerException("[WARN] Map coordinates is weird, X != Y. \n"));
    }

    int32_t numMapZones, mapBits, numFloats;
    mapBits = mapCoord[0] * mapCoord[1];
    _readBytes(4, &numMapZones);
    for (int i = 0; i < numMapZones; i++)
    {
        if (IS_HD(versionCode) || IS_DE(versionCode)) /// \todo 为什么不是11.76？
            _skip(2048 + mapBits * 2);
        else
            _skip(1275 + mapBits);
        _readBytes(4, &numFloats);
        _skip(numFloats * 4 + 4);
    }
    
    _readBytes(1, &allVisible);
    _readBytes(1, &fogOfWar);

    _mapBitmap = _curPos;
    uint32_t checkVal = *(uint32_t*)(_curPos + 7 * mapBits);
    if (IS_DE(versionCode)) {
        _mapTileType = (saveVersion >= 13.0299 || checkVal > 1000) ? 9 : 7;
    } else {
        _mapTileType = (_curPos[0] == 255) ? 4 : 2;
    }
    _skip(_mapTileType * mapBits);

    int32_t numData, numObstructions;
    _readBytes(4, &numData);
    _skip(4 + numData * 4);
    for (int i = 0; i < numData; i++)
    {
        _readBytes(4, &numObstructions);
        _skip(numObstructions * 8);
    }
    int32_t visibilityMapSize[2];
    _readBytes(8, visibilityMapSize);
    _skip(visibilityMapSize[0] * visibilityMapSize[1] * 4);
}

void DefaultAnalyzer::_replayAnalyzer() {
    _skip(12);
    _readBytes(4, &gameSpeed);
    _skip(29);
    _readBytes(2, &recPlayer);
    _readBytes(1, &numPlayers); /// \note gaia included
    if (!IS_AOK(versionCode)) { /// \todo AOK condition not tested
        _readBytes(1, &instantBuild);
        _readBytes(1, &cheatsEnabled);
    }
    _readBytes(2, &gameMode);
    _skip(58);
    if (IS_DE(versionCode)) _skip(8); /// \todo how about HD?
}

void DefaultAnalyzer::_AIAnalyzer() {
    _readBytes(4, &indcludeAI);
    if (!indcludeAI) return;

    _skip(2);
    uint16_t numAIStrings = *(uint16_t*)_curPos;
    _skip(6);
    for (uint32_t i = 0; i < numAIStrings; i++)
    {
        _skip(4 + *(int32_t*)_curPos);
    }
    
    if (IS_DE(versionCode)) {
        _skip(5);
    } else {
        _skip(4); /// \todo 验证一下各版本的情况
    }
    _expectBytes(
        patterns::AIdataUnknown,
        "[INFO] Found AI info, now before AI_DATA section, everything OK. \n", 
        "[WARN] Now in AI section, expecting 08 00 but disppointed. \n"
    );

    // AI Data
    if (IS_DE(versionCode)) { /// \todo this takes too long, not acceptable
        auto curItr = _header.begin() + (_curPos - _curStream);
        uint16_t rulesCnt = 0;

        // // Tricky skip
        // for (size_t i = 0; i < _DD_AICount; i++)
        // {
        //     patterns::AIDirtyFix[4] = i;
        //     curItr = findPosition(
        //         curItr, _header.end(),
        //         patterns::AIDirtyFix.begin(), 
        //         patterns::AIDirtyFix.end()
        //     );
        //     _curPos = &(*curItr);
        //     _skip(10);
        //     _readBytes(2, &rulesCnt);
        //     curItr += 200 * rulesCnt;
        //     cout << "rulesCnt: " << rulesCnt << endl;
        // }
        // _curPos = &(*curItr);
        
        // Dumb skip
        /// \todo 这里用了非常不确定的方法来跳过，没有充分验证。没有直接查找4096个0X00是出于性能考虑。
        patterns::FFs_500.resize(1000, 0x00);
        curItr = findPosition(
            curItr, _header.end(),
            patterns::FFs_500.begin(), 
            patterns::FFs_500.end()
        );
        _curPos = &(*curItr);
        _curPos += 4096 + 500;
    } else {
        int actionSize = 24; // See recanalyst
        int ruleSize = 16 + 16 * actionSize; // See recanalyst
        if (saveVersion > 11.9999) ruleSize += 0x180;

        for (uint16_t i = 0, numRules; i < 8; i++)
        {
            _skip(10);
            _readBytes(2, &numRules);
            if (numRules > 10000) {
                throw(AnalyzerException("[WARN] numRules in AI data > 1000 (maxRules is normally 10000). \n"));
            }
            _skip(4);
            for (int j = 0; j++ < numRules; _skip(ruleSize));
        }

        _skip(1448); // 104 + 320 + 1024 \note 这里我在104个字节后发现了2624个FF，也不知道为什么，应该是1344
        if (saveVersion >= 11.9599) _skip(1280); /// \todo 针对这个版本号边界要验证一下
        _skip(4096); // 4096个00
    }
    
    if (saveVersion >= 12.2999 && IS_HD(versionCode)) {
        _skip(4); /// \todo 这里应该是 recanalyst 里的，需要验证。考虑到不确定性，最好加一段异常后尝试查找地图坐标位置的代码。
    }
}

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
            throw(AnalyzerException("[ALERT] expecting 00 00 00 00 but disppointed. \n"));
        _skip(4);
    } else {
        if (*(uint32_t*)_curPos != -2)
            throw(AnalyzerException("[ALERT] expecting FE FF FF FF but disppointed. \n"));
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

int DefaultAnalyzer::_setVersionCode() {
    /// \todo Every condition needs to be tested!
    if (_bytecmp(versionStr, "TRL 9.3", 8)) {
        return versionCode == AOK ? AOKTRIAL : AOCTRIAL;
    }
    if (_bytecmp(versionStr, "VER 9.3", 8)) return versionCode = AOK;
    if (_bytecmp(versionStr, "VER 9.4", 8)) {
        if (logVersion == 3) return versionCode = AOC10;
        if (logVersion == 5 || saveVersion >= 12.9699) return versionCode = DE;
        if (saveVersion >= 12.4999) return versionCode = HD50_6;
        if (saveVersion >= 12.4899) return versionCode = HD48;
        if (saveVersion >= 12.3599) return versionCode = HD46_7;
        if (saveVersion >= 12.3399) return versionCode = HD43;
        if (saveVersion > 11.7601) return versionCode = HD;
        if (logVersion == 4) return versionCode = AOC10C;
        return versionCode = AOC;
    }
    if (_bytecmp(versionStr, "VER 9.5", 8)) return versionCode = AOFE21;
    if (_bytecmp(versionStr, "VER 9.8", 8)) return versionCode = USERPATCH12;
    if (_bytecmp(versionStr, "VER 9.9", 8)) return versionCode = USERPATCH13;
    if (_bytecmp(versionStr, "VER 9.A", 8)) return versionCode = USERPATCH14RC1;
    if (_bytecmp(versionStr, "VER 9.B", 8)) return versionCode = USERPATCH14RC2;
    if (_bytecmp(versionStr, "VER 9.C", 8) || _bytecmp(versionStr, "VER 9.D", 8)) return versionCode = USERPATCH14;
    if (_bytecmp(versionStr, "VER 9.E", 8) || _bytecmp(versionStr, "VER 9.F", 8)) return versionCode = USERPATCH15;
    if (_bytecmp(versionStr, "MCP 9.F", 8)) return versionCode = MCP;

    // If none above match:
    status = "fail";
    message.append("Detected unsupported game version.");
    versionCode = UNSUPPORTED;
    throw(AnalyzerException(message));
}

int DefaultAnalyzer::_inflateRawHeader()
{
    int ret;
    unsigned have;
    z_stream strm;
    uint8_t in[ZLIB_CHUNK];
    uint8_t out[ZLIB_CHUNK];

    _header.reserve(HEADER_INIT);

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        _f.read((char*)&in, ZLIB_CHUNK);
        strm.avail_in = _f.gcount();
        if (!_f.good()) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = ZLIB_CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            // assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }

            have = ZLIB_CHUNK - strm.avail_out;
            _header.insert(
                _header.end(),
                out,
                out + have
            );
            if (!_f.good()) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

string DefaultAnalyzer::generateMap(const string& path, bool hd) {
    return path;
}

void DefaultAnalyzer::extract(
    const string& headerPath  = "header.dat", 
    const string& bodyPath    = "body.dat"
) const
{
    ofstream headerOut(headerPath, ofstream::binary);
    ofstream bodyOut(bodyPath, ofstream::binary);

    headerOut.write((char*)_header.data(), _header.size());
    bodyOut.write((char*)_body.data(), _body.size());
    
    headerOut.close();
    bodyOut.close();
}

void DefaultAnalyzer::_expectBytes(
        const vector<uint8_t>& pattern,
        string good, string warn,
        bool skip,
        bool throwExpt
    ) {
        if (_bytecmp(_curPos, pattern.data(), pattern.size())) {
            message.append(good);
        } else {
            message.append(warn);
            if(throwExpt) throw(AnalyzerException(warn));
        }
        if(skip) _curPos += pattern.size();
}

bool DefaultAnalyzer::_findEncodingPattern(const char* pattern, std::string& mapName, size_t patternLen)
{
    size_t pos, posEnd;

    if (string::npos != (pos = instructions.find(pattern)))
    {
        posEnd = instructions.find('\n', pos + patternLen);
        if (string::npos != posEnd)
            embededMapName = instructions.substr(pos + patternLen, posEnd - pos - patternLen);

        return true;
    }

    return false;
}

/**
 * \todo aoc-mgz 中有更多关于编码和语言的关键字映射关系，可以套用过来。
 * 
 */
void DefaultAnalyzer::_guessEncoding()
{

    if (_findEncodingPattern(patterns::zh_pattern, embededMapName, size(patterns::zh_pattern)))
    {
        rawEncoding = "cp936";
    }
    else if (_findEncodingPattern(patterns::zh_utf8_pattern, embededMapName, size(patterns::zh_utf8_pattern)))
    {
        rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(patterns::zh_wide_pattern, embededMapName, size(patterns::zh_wide_pattern)))
    {
        rawEncoding = "cp936";
    }
    else if (_findEncodingPattern(patterns::zh_tw_pattern, embededMapName, size(patterns::zh_tw_pattern)))
    {
        rawEncoding = "cp950";
    }
    else if (_findEncodingPattern(patterns::br_pattern, embededMapName, size(patterns::br_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::de_pattern, embededMapName, size(patterns::de_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::en_pattern, embededMapName, size(patterns::en_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::es_pattern, embededMapName, size(patterns::es_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::fr_pattern, embededMapName, size(patterns::fr_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::it_pattern, embededMapName, size(patterns::it_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::jp_pattern, embededMapName, size(patterns::jp_pattern)))
    {
        rawEncoding = "cp932";
    }
    else if (_findEncodingPattern(patterns::jp_utf8_pattern, embededMapName, size(patterns::jp_utf8_pattern)))
    {
        rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(patterns::ko_pattern, embededMapName, size(patterns::ko_pattern)))
    {
        rawEncoding = "cp949";
    }
    else if (_findEncodingPattern(patterns::ko_utf8_pattern, embededMapName, size(patterns::ko_utf8_pattern)))
    {
        rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(patterns::ru_pattern, embededMapName, size(patterns::ru_pattern)))
    {
        rawEncoding = "windows-1251";
    }
    else if (_findEncodingPattern(patterns::ru_utf8_pattern, embededMapName, size(patterns::ru_utf8_pattern)))
    {
        rawEncoding = "windows-1251";
    }
    else if (_findEncodingPattern(patterns::nl_pattern, embededMapName, size(patterns::nl_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (IS_HD(versionCode) || IS_DE(versionCode))
    {
        rawEncoding = "utf-8";
    }
    else if (rawEncoding.size() == 0)
    {
        rawEncoding = "gbk";
    }

    if (_encodingConverter == nullptr)
        _encodingConverter = new EncodingConverter(outEncoding, rawEncoding);
}