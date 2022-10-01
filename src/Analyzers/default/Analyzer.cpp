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

#include <filesystem>
#include <sstream>

#include "Analyzer.h"
#include "TileStructures.h"
#include "utils.h"

using namespace std;

void DefaultAnalyzer::run() {
    auto p = filesystem::path(path);

    if (!filesystem::exists(path))
    {
        status = "fail";
        message.append(path + " does not exists. \n");
        throw(ParserException(message));
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
        throw(ParserException(message));
    }
    
    // Try to extract header&body streams
    if (!_locateStreams())
    {
        status = "fail";
        message.append("Failed to unzip raw header data. \n");
        throw(ParserException(message));
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
//              ┌───────────────┐
//       ┌──────┤version        │
//       │      ├───────────────┤   Inflated header data mainly
//       │      │HD/DE header   │   composed of these parts.
//       │      ├───────────────┤
//       │      │AI data        │   Some of them can be accessed
//       │      ├───────────────┤   by sequent read easily.
//       │      │Replay         │
//       │      ├───────────────┤   Some cannot. We need do some
//       │      │Map            │   search to reach those parts.
//       │      ├───────────────┤
//       │      │Start info    ◄│   by lichifeng, 2022/10/1
//    Inflated  ├───────────────┤
//    header    │Achievement    │
//    data      ├───────────────┤
//    structure │Scenario      ◄│
//       │      ├───────────────┤
//       │      │Victory        │
//       │      ├───────────────┤
//       │      │Unknown        │
//       │      ├───────────────┤
//       │      │Disabled      ◄│
//       │      ├───────────────│
//       │      │Game settings ◄│
//       │      ├───────────────│
//       │      │Trigger info  ◄│
//       │      ├───────────────┤
//       └──────┤Lobby          │
//              └───────────────┘

    // ************
    // * Phase 1: *
    // ************
    //   Reach sections which can reach easily(no search needed).
    //   1-1: Version
    _switchStream(BODY_STRM);
    _readBytes(4, &logVersion);
    _switchStream(HEADER_STRM);

    _readBytes(8, versionStr);
    _readBytes(4, &saveVersion);
    _setVersionCode();

    //   1-2: HD/DE-specific data
    if (IS_DE(versionCode)) {
        _headerDEAnalyzer();
    } else if (IS_HD(versionCode) && saveVersion > 12.3401) {
        /// \todo is this right cutoff point?? .mgx2 related?? see _gameSettingsAnalyzer()
        _headerHDAnalyzer();
    }
    
    //   1-3: AI
    _AIAnalyzer();

    //   1-4: Replay
    _replayAnalyzer();

    //   1-5: Map
    _mapDataAnalyzer();

    //   1-6: Find Startinfo
    _findStartInfoStart();

    // ************
    // * Phase 2: *
    // ************
    //   Find some key positions
    //   2-1: Trigger info start position
    _findTriggerInfoStart();

    //   2-2: Game settings start position. Need 2-1
    _findGameSettingsStart();

    //   2-3：Disables start position. Need 2-1
    _findDisablesStart();

    //   2-4: Game settings part, player names first appears here (before HD/DE
    //   versions). Need 2-2
    _gameSettingsAnalyzer();

    //   2-5: Search initial player data postion. Need 2-4
    _findInitialPlayersDataPos();

    //   2-6: Skip victory-related data. Need 2-3
    _findVictoryStart();

    //   2-7: Find&Skip scenario data. Need 2-6
    //   Nothing valuable here except a filename.
    //   What is really needed is instructions data after this section,
    //   Which is critical data to detect file encoding.
    _findScenarioHeaderStart();
    _scenarioHeaderAnalyzer();

    //   2-8: Skip trigger info. Need 2-1
    //   This is useless data, but need to get lobby start.
    _triggerInfoAnalyzer();

    // ************
    // * Phase 3: *
    // ************
    //   Do rest analyze. Following jobs is not necessarily ordered.

    //   3-1: Lobby data, lobby chat & some settings here. Need 2-8
    _lobbyAnalyzer();
    
    //   3-2: Victory
    _victorySettingsAnalyzer();
    
    //   3-3: Messages, ie. Instructions
    _messagesAnalyzer();

    //   3-4: Go back to player initial data position and rummage some useful pieces
    _startInfoAnalyzer();

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
            if(throwExpt) throw(ParserException(warn));
        }
        if(skip) _curPos += pattern.size();
}