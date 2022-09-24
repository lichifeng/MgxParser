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

    // Log current stage
    status = "good";
    message.append("Successfully extracted header&body streams from this file. Ready for data analyzing. \n");
    
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

    // Start data analyzing
    _analyze();

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
    if (IS_HD(versionCode) && saveVersion > 12.3401) {
        _headerHDAnalyzer();
    }

    if (IS_DE(versionCode)) {
        _printHex(8);
    }
    _readBytes(4, &indcludeAI);
}

void DefaultAnalyzer::_headerHDAnalyzer() {
    int16_t  tmpInt16;
    uint8_t* tmpPos;

    _readBytes(4, &HD_version);
    if (HD_version - 1006 < 0.0001) versionCode = HD57;
    _readBytes(4, &HD_internalVersion);
    _readBytes(4, &HD_gameOptionsVersion);
    _readBytes(4, &HD_DLCCount);
    _skip(HD_DLCCount * 4);
    _readBytes(4, &HD_datasetRef);
    _readBytes(4, &HD_difficultyID);
    _readBytes(4, &HD_selectedMapID);
    _readBytes(4, &HD_resolvedMapID);
    _readBytes(4, &HD_revealMap);
    _readBytes(4, &HD_victoryTypeID);
    _readBytes(4, &HD_startingResourcesID);
    _readBytes(4, &HD_startingAgeID);
    _readBytes(4, &HD_endingAgeID);
    if (HD_version >= 1005.9999) _readBytes(4, &HD_gameType);

    /// \note Next 4 bytes should be: a3 5f 02 00. Mark a simple sign here.
    if (_bytecmp(_curPos, patterns::HDseparator, sizeof(patterns::HDseparator))) {
        message.append("Analyzing HD-specific data section in header stream. \n");
    } else {
        message.append("Unexpected validating pattern HD-specific data section in header stream. \n");
    }
    _skip(4);
    if (HD_version == 1000) {
        _readBytes(2, &tmpInt16);
        // Next 2 bytes should be: [ 60 0A ]
        _skip(2);
        _readBytes(tmpInt16, HD_ver1000MapName.data());

        _skipHDString();
    }
    // Next 4 bytes should be: a3 5f 02 00
    _skip(4);
    _readBytes(4, &HD_speed);
    _readBytes(4, &HD_treatyLength);
    _readBytes(4, &HD_populationLimit);
    _readBytes(4, &HD_numPlayers);
    _readBytes(4, &HD_unusedPlayerColor);
    _readBytes(4, &HD_victoryAmount);
    // Next 4 bytes should be: a3 5f 02 00
    _skip(4);
    _readBytes(1, &HD_tradeEnabled);
    _readBytes(1, &HD_teamBonusDisabled);
    _readBytes(1, &HD_randomPositions);
    _readBytes(1, &HD_allTechs);
    _readBytes(1, &HD_numStartingUnits);
    _readBytes(1, &HD_lockTeams);
    _readBytes(1, &HD_lockSpeed);
    _readBytes(1, &HD_multiplayer);
    _readBytes(1, &HD_cheats);
    _readBytes(1, &HD_recordGame);
    _readBytes(1, &HD_animalsEnabled);
    _readBytes(1, &HD_predatorsEnabled);
    /// \todo Next 4 bytes should be: a3 5f 02 00, but aoc-mgz indicates they are following data
    // _readBytes(1, &HD_turboEnabled);
    // _readBytes(1, &HD_sharedExploration);
    // _readBytes(1, &HD_teamPositions);
    // _skip(1); // Unknown byte
    _skip(4);
    if (HD_version == 1000) {
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
        if (HD_version >= 1005.9999) _skip(1);
        _skip(15);
        _skipHDString();
        _skip(1);
        if (HD_version >= 1004.9999) _skipHDString();
        _skipHDString();
        _skip(16);
        _readBytes(4, &test);
        if (check != test) versionCode = HD58;
        _curPos = tmpPos;

        // Read player data
        for (size_t i = 0; i < 8; i++)
        {
            _readBytes(4, &players[i].DLCID);
            _readBytes(4, &players[i].colorID);
            if (HD_version >= 1005.9999) _skip(1);
            _skip(2);
            _readBytes(4, &players[i].datCrc);
            _readBytes(1, &players[i].MPGameVersion);
            _readBytes(4, &players[i].teamIndex);
            _readBytes(4, &players[i].civID);
            _readHDString(players[i].AIType);
            _readBytes(1, &players[i].AICivNameIndex);
            if (HD_version >=1004.9999) _readHDString(players[i].AIName);
            _readHDString(players[i].name);
            _readBytes(4, &players[i].playerType);
            _readBytes(8, &players[i].steamID);
            _readBytes(4, &players[i].playerNumber);
            if (HD_version >= 1005.9999 
                && versionCode != HD50_6 
                && versionCode != HD57)
            {
                _readBytes(4, &players[i].HDRMRating);
                _readBytes(4, &players[i].HDDMRating);
            }
        }
        
        _readBytes(1, &HD_fogOfWar);
        _readBytes(1, &HD_cheatNotifications);
        _readBytes(1, &HD_coloredChat);
        _skip(13); /// 9 bytes + a3 5f 02 00
        _readBytes(1, &HD_isRanked);
        _readBytes(1, &HD_allowSpecs);
        _readBytes(4, &HD_lobbyVisibility);
        _readBytes(4, &HD_customRandomMapFileCrc);
        _readHDString(HD_customScenarioOrCampaignFile);
        _skip(8);
        _readHDString(HD_customRandomMapFile);
        _skip(8);
        _readHDString(HD_customRandomMapScenarionFile);
        _skip(8);
        HD_guid = hexStr(_curPos, 16, true); /// \todo should this map to gamehash or filehash?
        _readHDString(HD_lobbyName);
        _readHDString(HD_moddedDataset);
        HD_moddedDatasetWorkshopID = hexStr(_curPos, 4, true);
        if (HD_version >= 1004.9999)
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