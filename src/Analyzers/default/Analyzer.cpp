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
    _findInitialPlayersDataPos();

    // Go back to player initial data position and rummage some useful pieces
    _startInfoAnalyzer();

    // Trigger info is normally skipped
    _triggerInfoAnalyzer();

    // Analyze some misc data
    _lobbyAnalyzer();
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