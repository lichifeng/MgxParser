/**
 * \file       Analyzer.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-22
 * 
 * \copyright  Copyright (c) 2020-2022
 *  
 */
#pragma once

#define HEADER_INIT 4*1024*1024
#define HEADER_STRM 0
#define BODY_STRM 1

#include <fstream>
#include <cstddef>
#include <vector>
#include <array>
#include <cstring>
#include <iostream>
#include <iomanip>
#include "../BaseAnalyzer.h"
#include "version.h"
#include "../AnalyzerException.h"
#include "Player.h"

using namespace std;

/**
 * \brief      默认解析器
 *
 */
class DefaultAnalyzer: public BaseAnalyzer
{
public:
    DefaultAnalyzer() {}

    DefaultAnalyzer(const string& path) {
        this->path = path;
    }

    void run();
    string generateMap(const string&, bool);

    /**
     * \brief      Extract header&body streams into separate files
     * 
     * \param      headerPath       filename of generated header file
     * \param      body             filename of generated body file
     */
    void extract(const string&, const string&) const;

    string            path; ///< 录像文件输入路径
    string            filename; ///< 录像文件名（去除路径后）
    string            ext; ///< 录像文件扩展名
    uintmax_t         filesize; ///< 录像文件大小，单位是 bytes
   
    uint32_t          logVersion; ///< body 的前4个字节，与版本有关，可以识别A/C版
    char              versionStr[8]; ///< 代表游戏版本的原始字符串
    float             saveVersion; ///< \warning 小数点后有许多位，比较的时候注意要合理处理，比如 >11.76 要写成 >11.7599 这种
    int               versionCode; ///< 这是自己定义的一个值，所以类型定义的时候用 int 不用 int32_t，以示区分
    uint32_t          indcludeAI;

    // HD-specific data from header stream
    float             HD_version; ///< hd<=4.7: 1000; hd=5.8: 1006
    uint32_t          HD_internalVersion;
    uint32_t          HD_gameOptionsVersion;
    uint32_t          HD_DLCCount;
    uint32_t          HD_datasetRef; ///< \todo What's this?
    uint32_t          HD_difficultyID;
    uint32_t          HD_selectedMapID;
    uint32_t          HD_resolvedMapID;
    uint32_t          HD_revealMap;
    uint32_t          HD_victoryTypeID;
    uint32_t          HD_startingResourcesID;
    uint32_t          HD_startingAgeID;
    uint32_t          HD_endingAgeID;
    uint32_t          HD_gameType = -1; ///< Only when HD_version>=1006
    string            HD_ver1000MapName;
    string            HD_ver1000Unknown;
    float             HD_speed;
    uint32_t          HD_treatyLength;
    uint32_t          HD_populationLimit;
    uint32_t          HD_numPlayers;
    uint32_t          HD_unusedPlayerColor;
    uint32_t          HD_victoryAmount;
    uint8_t           HD_tradeEnabled;
    uint8_t           HD_teamBonusDisabled;
    uint8_t           HD_randomPositions;
    uint8_t           HD_allTechs;
    uint8_t           HD_numStartingUnits;
    uint8_t           HD_lockTeams;
    uint8_t           HD_lockSpeed;
    uint8_t           HD_multiplayer;
    uint8_t           HD_cheats;
    uint8_t           HD_recordGame;
    uint8_t           HD_animalsEnabled;
    uint8_t           HD_predatorsEnabled;
    // uint8_t           HD_turboEnabled;
    // uint8_t           HD_sharedExploration;
    // uint8_t           HD_teamPositions;

    array<Player, 8>  players {};
    uint8_t           HD_fogOfWar;
    uint8_t           HD_cheatNotifications;
    uint8_t           HD_coloredChat;
    uint8_t           HD_isRanked;
    uint8_t           HD_allowSpecs;
    uint32_t          HD_lobbyVisibility;
    uint32_t          HD_customRandomMapFileCrc;
    string            HD_customScenarioOrCampaignFile;
    string            HD_customRandomMapFile;
    string            HD_customRandomMapScenarionFile;
    string            HD_guid;
    string            HD_lobbyName;
    string            HD_moddedDataset;
    string            HD_moddedDatasetWorkshopID;
   
    string            playDate; ///< 游戏发生时间，对老录像只能推断 \todo 有时需要从上传时间来推断，是否放在更外层的类里面？
    string            status; ///< 解析完成类型：success, fail, partly, etc.
    string            message; ///< 对 \p status 的具体说明
    string            parseMode; ///< 解析模式：normal, verbose, etc. 可以在命令行中指定
    double            parseTime; ///< 解析耗时（毫秒）

protected:
    bool              _locateStreams(); ///< 对文件流进行处理，定位 header & body 的起始位置
    int               _inflateRawHeader(); ///< 解压 header 数据

    /**
     * \brief      切换当前工作的数据流（header 或者 body）
     * 
     * \param      stream              HEADER_STRM/BODY_STRM
     */
    inline void       _switchStream(uint8_t stream = HEADER_STRM)
    { 
        HEADER_STRM == stream ? _curStream = _header.data() : _curStream = _body.data(); 
        _curPos = _curStream;
    }

    /**
     * \brief      将当前位置往后 n 个字节的数据存储到一个变量上
     * 
     * \param      n                   往后读取的字节数
     * \param      dest                指向目标变量的指针
     */
    inline void        _readBytes(uintmax_t n, void* dest)
    {
        memcpy(dest, _curPos, n);
        _curPos += n;
    }

    inline void        _skipHDString()
    {
        int16_t l;
        _readBytes(2, &l);
        _skip(2 + l);
    } ///< Skip strings in HD header data

    inline void        _readHDString(string& s)
    {
        uint16_t l;
        uint8_t p[2] = {0x60, 0x0A};
        _readBytes(2, &l);
        if (!_bytecmp(_curPos, p, 2))
            throw(AnalyzerException("Meet unexpected pattern when reading an HD string."));
        _skip(2);
        ///< \todo 搞不懂哪里错了，是对象的初始化问题吗？
        _skip(l);
    } ///< Read HD string into s.

    void               _skip(uintmax_t n) { _curPos += n; } ///< Skip forward n bytes.

    void               _analyze(); ///< 从 header 开始进行分析

    int                _setVersionCode(); ///< 这个不是原始数据，是自己归纳出的一个值，后面判断版本有用

    /**
     * \brief      Compare two C-style strings or byte sequence.
     * 
     * \param      s                   string1
     * \param      pattern             string2
     * \return     true                string1 == string2
     * \return     false               string1 != string2
     */
    inline bool        _bytecmp(const void* s, const void* pattern, size_t n) const
    {
        return 0 == memcmp(s, pattern, n);
    }

    inline void        _printHex(int n) {
        ios_base::fmtflags f( cout.flags() );
        cout << "Now at position: " << _curPos - _curStream << endl \
            << "Next " << n << " bytes are: [ ";
        for (size_t i = 0; i < n; i++)
        {
            cout << hex << setfill('0') << uppercase \
                << setw(2) << (int)_curPos[i] << " ";
        }
        cout << "]" << endl;
        cout.flags( f );
    }

    void               _headerHDAnalyzer(); ///< 分析 header 中的 HD 部分信息

    ifstream           _f; ///< 录像文件的原始流
    uintmax_t          _bodySize; ///< body 数据的长度
    vector<uint8_t>    _body; ///< body stream
    vector<uint8_t>    _header; ///< 解压后的 header 数据
 
    uint8_t*           _curPos; ///< 当前读取数据的指针
    uint8_t*           _curStream; ///< 当前使用的数据流
};
