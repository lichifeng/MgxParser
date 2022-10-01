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
#define PrintHEX(n) _printHex(n, __FILE__, __LINE__)

#include <fstream>
#include <cstddef>
#include <vector>
#include <array>
#include <cstring>
#include <iostream>
#include <iomanip>
#include <iterator>

#include "../../Logger.h"
#include "../../EncodingConverter.h"
#include "../../ParserException.h"
#include "../BaseAnalyzer.h"
#include "utils.h"

#include "Player.h"

using namespace std;

/**
 * \brief      默认解析器
 *
 */
class DefaultAnalyzer: public BaseAnalyzer
{
public:
    ~DefaultAnalyzer() {
        delete _encodingConverter;
    };

    DefaultAnalyzer(const string& path) {
        this->path = path;
        chat.resize(50);
    }

    void run();
    string generateMap(const string&, bool);

    inline string& fixEncoding(string& s) {
        if (nullptr == _encodingConverter) return s;
        _encodingConverter->convert(s, s);
        return s;
    } ///< Convert a string from record file encoding to specified output encoding. Input string not necessarily a member of this class, so its a public function.

    /**
     * \brief      Extract header&body streams into separate files
     * 
     * \param      headerPath       filename of generated header file
     * \param      body             filename of generated body file
     */
    void extract(const string, const string) const;

    Logger*           logger = nullptr;
    string            path; ///< 录像文件输入路径
    string            filename; ///< 录像文件名（去除路径后）
    string            ext; ///< 录像文件扩展名
    uintmax_t         filesize; ///< 录像文件大小，单位是 bytes

    /**
     * \details      设定用于startinfo 中玩家信息搜索时的特征字节长度。在设定为6
     * 时，一个包含 AI 信息的 DE 录像文件解析花费 210ms；在设定为 5 时，花费 92ms；
     * 在设定为4时，花费126MS；在设定为3时，花费125MS；在设定为2时，花费149MS；
     * 在设定为1时，花费228MS。
     */
    uint16_t          trailBytes = 5;
    uint32_t          easySkipBase = 35100;
    uint32_t          triggerStartSearchRange = 11;
    uint32_t          ZLIB_CHUNK = 8192;
   
    uint32_t          logVersion; ///< body 的前4个字节，与版本有关，可以识别A/C版
    char              versionStr[8]; ///< 代表游戏版本的原始字符串
    float             saveVersion; ///< \warning 小数点后有许多位，比较的时候注意要合理处理，比如 >11.76 要写成 >11.7599 这种
    VERSIONCODE       versionCode; ///< 这是自己定义的一个值，所以类型定义的时候用 int 不用 int32_t，以示区分
    uint32_t          indcludeAI;

    // HD/DE-specific data from header stream
    uint32_t          DE_build = 0; ///< Recent steam version of DE: 66692
    uint32_t          DE_timestamp;
    float             DD_version; ///< hd<=4.7: 1000; hd=5.8: 1006
    uint32_t          DD_internalVersion;
    uint32_t          DD_gameOptionsVersion;
    uint32_t          DD_DLCCount;
    uint32_t          DD_datasetRef; ///< \todo What's this?
    uint32_t          DD_difficultyID;
    uint32_t          DD_selectedMapID;
    uint32_t          DD_resolvedMapID;
    uint32_t          revealMap = 999; ///< 0x00:通常，0x01:已开发，0x02:全部显示, 0x03:no fog
    uint32_t          DD_victoryTypeID;
    uint32_t          DD_startingResourcesID;
    uint32_t          DD_startingAgeID;
    uint32_t          DD_endingAgeID;
    uint32_t          DD_gameType = -1; ///< Only when DD_version>=1006
    string            HD_ver1000MapName;
    string            HD_ver1000Unknown;
    float             DD_speed = -1.0;
    uint32_t          DD_treatyLength;
    uint32_t          DD_populationLimit;
    uint32_t          DD_numPlayers = 0; ///< \note Gaia not included
    uint32_t          DD_unusedPlayerColor;
    uint32_t          DD_victoryAmount;
    uint8_t           DD_tradeEnabled;
    uint8_t           DD_teamBonusDisabled;
    uint8_t           DD_randomPositions;
    uint8_t           DD_allTechs;
    uint8_t           DD_numStartingUnits;
    uint8_t           DD_lockTeams;
    uint8_t           DD_lockSpeed;
    uint8_t           DD_multiplayer;
    uint8_t           DD_cheats;
    uint8_t           DD_recordGame;
    uint8_t           DD_animalsEnabled;
    uint8_t           DD_predatorsEnabled;
    uint8_t           DD_turboEnabled;
    uint8_t           DD_sharedExploration;
    uint8_t           DD_teamPositions;
    uint32_t          DD_subGameMode;
    uint32_t          DD_battleRoyaleTime;
    uint8_t           DD_handicap;

    array<Player, 9>  players;
    uint8_t           DD_fogOfWar; ///< \todo combine with lobby one(4 bytes)
    uint8_t           DD_cheatNotifications;
    uint8_t           DD_coloredChat;
    uint8_t           DD_isRanked;
    uint8_t           DD_allowSpecs;
    uint32_t          DD_lobbyVisibility;
    uint8_t           DE_hiddenCivs;
    uint8_t           DE_matchMaking;
    uint32_t          DE_specDely;
    uint8_t           DE_scenarioCiv;
    string            DE_RMSCrc;
    uint32_t          HD_customRandomMapFileCrc;
    string            HD_customScenarioOrCampaignFile;
    string            HD_customRandomMapFile;
    string            HD_customRandomMapScenarionFile;
    string            DD_guid;
    string            DD_lobbyName;
    string            DD_moddedDataset;
    string            HD_moddedDatasetWorkshopID;
    uint64_t          DE_numAIFiles;

    // data from replay section
    uint32_t          gameSpeed; ///< \todo If de/hd, use data from de/hd-specific data
    uint16_t          recPlayer; ///< \todo index or number of pov?? verify this.
    uint8_t           numPlayers; ///< \todo gaia included, DD_numPlayers first??
    uint8_t           instantBuild;
    uint8_t           cheatsEnabled; ///< \todo DD_cheats??
    uint16_t          gameMode; ///< \todo use DD_multiplayer first?? or DD_subGameMode??

    // data from map data section
    int32_t           mapCoord[2];
    uint8_t           allVisible;
    uint8_t           fogOfWar; ///< \todo 有三处出现这个值，到底取哪一处？

    // data from start info
    uint32_t          restoreTime;

    // data from scenario header
    float             scenarioVersion;
    string            scenarioFilename;
    string            instructions;

    // victory conditions
    uint32_t          victoryIsConquest;
    uint32_t          victoryRelics;
    uint32_t          victoryExplored;
    uint32_t          victoryAnyOrAll;
    uint32_t          victoryMode;
    uint32_t          victoryScore;
    uint32_t          victoryTime;

    // game settings
    uint32_t          mapID;
    uint32_t          difficultyID;
    uint32_t          lockTeams;

    // lobby settings
    uint32_t          mapSize;
    uint32_t          populationLimit; ///< \todo combine with DD_pop...
    uint8_t           gameType; ///< \todo 这个可以和上面的合并吗？
    uint8_t           lockDiplomacy; ///< \todo 和lockteams可以合并吗？
    uint8_t           treatyLength; ///< \todo 可以和上面的合并吗？
    vector<string>    chat;
    int32_t           mapSeed;

    // other data
    string            embededMapName; ///< Map name extracted from instructions, not mapped with raw number
    
    string            rawEncoding;
    string            outEncoding = "UTF-8";
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

    inline size_t  _distance() { return _curPos - _header.data(); }
    inline size_t  _remainBytes() { return (_header.size() >= _distance()) ? (_header.size() - _distance()) : 0; }

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

    inline void       _skipPascalString(bool lengthLong = false) {
        uint32_t lenStr = lengthLong ? *(uint32_t*)_curPos : *(uint16_t*)_curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 1000) throw(ParserException("[WARN] Got an unexpected string length. \n"));

        _skip(lenInt + lenStr);
        
         
    } ///< 跳过最常见的字符串类型

    void              _readPascalString(string& s, bool convertEncodeing = true, bool lengthLong = false) {
        uint32_t lenStr = lengthLong ? *(uint32_t*)_curPos : *(uint16_t*)_curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 1000) throw(ParserException("[WARN] Got an unexpected string length. \n"));

        _skip(lenInt);
        s.assign((char*)_curPos, lenStr);
        _skip(lenStr);

        if (convertEncodeing && _encodingConverter != nullptr) {
            fixEncoding(s);
        }
    }

    inline void        _skipDEString()
    {
        int16_t l[2]; // 0x60 0x0a int16_t
        _readBytes(4, l);
        _skip(l[1]);
    } ///< Skip strings in DE header data

    inline void        _skipHDString()
    {
        int16_t l;
        _readBytes(2, &l);
        _skip(2 + l);
    } ///< Skip strings in HD header data

    inline void        _readDEString(string& s)
    {
        uint16_t l;
        uint8_t p[2] = {0x60, 0x0A};
        if (!_bytecmp(_curPos, p, 2))
            throw(ParserException("[ALERT] Meet unexpected pattern when reading an DE string. \n"));
        _skip(2);
        _readBytes(2, &l);
        s.assign((char*)_curPos, l);
        _skip(l);
    } ///< Read DE string into s.

    inline void        _readHDString(string& s)
    {
        uint16_t l;
        uint8_t p[2] = {0x60, 0x0A};
        _readBytes(2, &l);
        if (!_bytecmp(_curPos, p, 2))
            throw(ParserException("[ALERT] Meet unexpected pattern when reading an HD string. \n"));
        _skip(2);
        s.assign((char*)_curPos, l);
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

    void               _expectBytes(
        const vector<uint8_t>& pattern,
        string good, string warn, 
        bool skip = true,
        bool throwExpt = true
    );

    inline void        _printHex(size_t n, string file, size_t line) {
        if (nullptr == logger) return;
        if (_remainBytes() <= n) n = _remainBytes();

        logger->logHex(
            n,
            _header.begin() + _distance(),
            _distance(),
            file,
            line
        );
    }

    /**
     * \brief      Used to find key bytes represents "Map name: " to determine
     * string encoding of record.
     * 
     * \param      pattern             Key characters represents "map name:" in
     * different languages
     * \param      mapName             reference to embededMapName
     * \param      patternLen          
     * \return     true                
     * \return     false               
     */
    bool                         _findEncodingPattern(const char* pattern, std::string& mapName, size_t patternLen);
    void                         _guessEncoding();

    void                         _headerHDAnalyzer(); ///< 分析 header 中的 HD 部分信息
    void                         _headerDEAnalyzer(); ///< 分析 header 中的 DE 部分信息
    void                         _AIAnalyzer(); ///< 分析 header 中的 AI 部分信息
    void                         _replayAnalyzer();
    void                         _mapDataAnalyzer();
    void                         _findStartInfoStart();
    void                         _findTriggerInfoStart();
    void                         _findDisablesStart();
    void                         _findGameSettingsStart();
    void                         _findVictoryStart();
    void                         _findScenarioHeaderStart();
    void                         _scenarioHeaderAnalyzer();
    void                         _messagesAnalyzer();
    void                         _victorySettingsAnalyzer();
    void                         _gameSettingsAnalyzer();
    void                         _findInitialPlayersDataPos();
    void                         _startInfoAnalyzer();
    void                         _triggerInfoAnalyzer();
    void                         _lobbyAnalyzer();
             
    ifstream                     _f; ///< 录像文件的原始流
    uintmax_t                    _bodySize; ///< body 数据的长度
    vector<uint8_t>              _body; ///< body stream
    vector<uint8_t>              _header; ///< 解压后的 header 数据
             
    uint8_t*                     _curPos; ///< 当前读取数据的指针 \todo 为什么不直接用迭代器呢
    uint8_t*                     _curStream; ///< 当前使用的数据流
    /// \todo 感觉还是把指针改用 iterator 比较舒服，然后加一个告知当前位置的方法。
    
    // vector<uint8_t>::iterator    _cursor; ///< An iterator represents current working position in data stream.
    // inline void                  _forward(int n) { advance(_cursor, n); } ///< Move current position forward by n bytes.
    
    uint32_t                     _DD_AICount = 0; ///< \note used to skip AI section
    uint8_t*                     _startInfoPatternTrail;
    void*                        _mapBitmap;
    uint8_t                      _mapTileType = 0; ///< \note 7: DETile1; 9: DETile2; 4: Tile1; 2: TileLegacy. This value is size of structure.

    uint8_t*                     _startInfoPos;
    uint8_t*                     _triggerInfoPos;
    uint8_t*                     _gameSettingsPos;
    uint8_t*                     _disablesStartPos;
    uint8_t*                     _victoryStartPos;
    uint8_t*                     _scenarioHeaderPos;
    uint8_t*                     _messagesStartPos;

    EncodingConverter*           _encodingConverter = nullptr;
};
