/**
 * \file       Analyzer.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-10-02
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#pragma once

#define HEADER_INIT 4 * 1024 * 1024
#define HEADER_STRM 0
#define BODY_STRM 1
#define PrintHEX(n) _printHex(n, __FILE__, __LINE__)
#define UINT32_INIT 4294967295
#define FLOAT_INIT -20000.0

#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

#include "../../EncodingConverter.h"
#include "../../Logger.h"
#include "../../ParserException.h"
#include "../BaseAnalyzer.h"
#include "Player.h"
#include "Chat.h"
#include "utils.h"

using namespace std;

/**
 * \brief      默认解析器，可以通过继续它来增加新的解析器。例如可以用来增加一个快速生成地图的版本，省略不必要的解析。
 *
 */
class DefaultAnalyzer : public BaseAnalyzer
{
public:
    ~DefaultAnalyzer() { delete _encodingConverter; };

    DefaultAnalyzer(const string &path) : path(path) { chat.resize(50); }

    void run();

    void generateMap(const string path, uint32_t width = 300, uint32_t height = 150, bool hd = false);

    ///< Convert a string from record file encoding to specified output
    ///< encoding. Input string not necessarily a member of this class, so its
    ///< a public function.

    /**
     * \brief      将输入的字符串转换成设定的编码（一般是UTF-8）
     *
     * \param      s                   要转换的字符串
     * \return     string&             转换后的字符串（是输入字符串的一个引用）
     */
    inline string &fixEncoding(string &s)
    {
        if (nullptr == _encodingConverter)
        {
            logger->warn(
                "Try to convert a string before converter initialized @{} of header.",
                _distance());
            return s;
        }
        _encodingConverter->convert(s, s);
        return s;
    }

    /**
     * \brief      Extract header&body streams into separate files
     *
     * \param      headerPath       filename of generated header file
     * \param      body             filename of generated body file
     */
    void extract(const string, const string) const;

    Logger *logger = nullptr;

    // File-related members
    string path;            ///< 录像的路径
    string filename;        ///< 录像的文件名
    string ext;             ///< 录像的扩展名 \todo 要检查扩展名
    uintmax_t filesize = 0; ///< 录像文件大小，单位是 bytes

    // Some config parameters
    uint16_t trailBytes = 5;               ///< 设定用于startinfo中玩家信息搜索时的特征字节长度，影响速度
    uint32_t easySkipBase = 35100;         ///< 在startinfo中搜索时可以放心跳过的字节长度
    uint32_t triggerStartSearchRange = 19; ///< 查找triggerinfo位置时的参数，较早版本有0和1，DE中一般为11，如果找不到可以考虑放大范围试试
    uint32_t ZLIB_CHUNK = 8192;            ///< ZLIB解压时的参数
    uint32_t scenarioSearchSpan = 8000;    ///< usually 5500~6500

    // Version-related members
    uint32_t logVersion;     ///< body 的前4个字节，与版本有关，可以识别A/C版
    char versionStr[8];      ///< 代表游戏版本的原始字符串
    float saveVersion;       ///< \warning float有精度，进行比较的时候注意要合理处理，比如>11.76要写成>11.7599这种
    VERSIONCODE versionCode; ///< 这是自己定义的一个值，用于简化版本判断
    uint32_t indcludeAI;

    // HD/DE-specific data from header stream
    uint32_t DE_build = 0;         ///< In recent(2022-10) steam version of DE: 66692
    uint32_t DE_timestamp;         ///< 游戏时间，只有DE版本中有
    float DD_version = FLOAT_INIT; ///< hd<=4.7: 1000; hd=5.8: 1006
    uint32_t DD_internalVersion;   ///< DE中是1000
    uint32_t DD_gameOptionsVersion;
    uint32_t DD_DLCCount;
    uint32_t DD_datasetRef; ///< \todo What's this?
    uint32_t DD_difficultyID;
    uint32_t DD_selectedMapID;
    uint32_t DD_resolvedMapID;
    uint32_t revealMap = UINT32_INIT; ///< 0x00:通常，0x01:已开发，0x02:全部显示, 0x03:no fog
    uint32_t DD_victoryTypeID;
    uint32_t DD_startingResourcesID;
    uint32_t DD_startingAgeID;
    uint32_t DD_endingAgeID;
    uint32_t DD_gameType = UINT32_INIT; ///< Only when DD_version>=1006
    string HD_ver1000MapName;
    // string HD_ver1000Unknown;
    float DD_speed = FLOAT_INIT;
    uint32_t DD_treatyLength;
    // uint32_t DD_populationLimit = UINT32_INIT; ///< Store this value in populationLimit
    uint32_t DD_numPlayers = UINT32_INIT; ///< \note Gaia not included
    uint32_t DD_unusedPlayerColor;
    uint32_t DD_victoryAmount;
    uint8_t DD_tradeEnabled;
    uint8_t DD_teamBonusDisabled;
    uint8_t DD_randomPositions;
    uint8_t DD_allTechs;
    uint8_t DD_numStartingUnits;
    uint8_t DD_lockTeams;
    uint8_t DD_lockSpeed;
    uint8_t DD_multiplayer;
    uint8_t DD_cheats;
    uint8_t DD_recordGame;
    uint8_t DD_animalsEnabled;
    uint8_t DD_predatorsEnabled;
    uint8_t DD_turboEnabled;
    uint8_t DD_sharedExploration;
    uint8_t DD_teamPositions;
    uint32_t DD_subGameMode;
    uint32_t DD_battleRoyaleTime;
    uint8_t DD_handicap; ///< 让分

    array<Player, 9> players;
    // uint8_t DE_fogOfWar;
    uint8_t DD_cheatNotifications;
    uint8_t DD_coloredChat;
    uint8_t DD_isRanked;
    uint8_t DD_allowSpecs;
    uint32_t DD_lobbyVisibility;
    uint8_t DE_hiddenCivs;
    uint8_t DE_matchMaking;
    uint32_t DE_specDely;
    uint8_t DE_scenarioCiv;
    string DE_RMSCrc;
    uint32_t HD_customRandomMapFileCrc;
    string HD_customScenarioOrCampaignFile;
    string HD_customRandomMapFile;
    string HD_customRandomMapScenarionFile;
    string DD_guid;
    string DD_lobbyName;
    string DD_moddedDataset;
    string HD_moddedDatasetWorkshopID;
    uint64_t DE_numAIFiles;

    // data from replay section
    uint32_t gameSpeed; ///< \todo If de/hd, use data from de/hd-specific data
    uint16_t recPlayer; ///< \todo index or number of pov?? verify this.
    uint8_t numPlayers; ///< \todo gaia included, DD_numPlayers first??
    uint8_t instantBuild;
    uint8_t cheatsEnabled; ///< \todo DD_cheats??
    // uint16_t gameMode;     ///< \note Unknown.

    // data from map data section
    int32_t mapCoord[2];
    void *mapDataPtr;
    uint8_t allVisible;
    // uint8_t fogOfWar; ///< \note Use fogOfWar in lobby

    // data from start info
    uint32_t restoreTime;

    // data from scenario header
    float scenarioVersion;
    string scenarioFilename;
    string instructions;

    // victory conditions
    uint32_t victoryIsConquest;
    uint32_t victoryRelics;
    uint32_t victoryExplored;
    uint32_t victoryAnyOrAll;
    uint32_t victoryMode;
    uint32_t victoryScore;
    uint32_t victoryTime;

    // game settings
    uint32_t mapID;
    uint32_t difficultyID;
    uint32_t lockTeams;

    // lobby settings
    uint32_t fogOfWar;
    uint32_t mapSize;
    uint32_t populationLimit = UINT32_INIT;
    uint8_t gameType;
    uint8_t lockDiplomacy; ///< \note DE/HD数据中还有一个类似的
    uint8_t treatyLength;  ///< \note DE/HD数据中还有一个类似的
    vector<Chat> chat;
    int32_t DE_mapSeed;

    // other data
    string embededMapName; ///< Map name extracted from instructions, not mapped with raw number

    string rawEncoding = "GBK";
    string outEncoding = "UTF-8";
    string playDate;  ///< 游戏发生时间，对老录像只能推断 \todo
                      ///< 有时需要从上传时间来推断，是否放在更外层的类里面？
    string status = "good";    ///< 解析完成类型：good, warning, fatal, etc.
    string message;   ///< 对 \p status 的具体说明
    string parseMode = "MgxParser Normal"; ///< 解析模式：normal, verbose, etc. 可以在命令行中指定
    double parseTime; ///< 解析耗时（毫秒）

protected:
    bool _locateStreams(); ///< 对文件流进行处理，定位 header & body 的起始位置

    int _inflateRawHeader(); ///< 解压 header 数据

    /**
     * \brief      切换当前工作的数据流（header 或者 body）
     *
     * \param      stream              HEADER_STRM/BODY_STRM
     */
    inline void _switchStream(uint8_t stream = HEADER_STRM)
    {
        (HEADER_STRM == stream) ? _curStream = _header.data()
                                : _curStream = _body.data();
        _curPos = _curStream;
    }

    inline size_t _distance() { return _curPos - _header.data(); } ///< 获取当前读取位置（相对于STREAM开头）

    inline size_t _remainBytes()
    {
        return (_header.size() >= _distance()) ? (_header.size() - _distance()) : 0;
    } ///< 获取当前位置之后剩余的字节数

    /**
     * \brief      将当前位置往后 n 个字节的数据存储到一个变量上
     *
     * \param      n                   往后读取的字节数
     * \param      dest                指向目标变量的指针
     */
    inline void _readBytes(size_t n, void *dest)
    {
        memcpy(dest, _curPos, n);
        _curPos += n;
    }

    /**
     * \brief      跳过“长度（2字节/4字节）+字符串内容”格式的字符串
     *
     * \param      lengthLong          长度是用4个字节（true）还是2个字节（false）表示
     */
    inline void _skipPascalString(bool lengthLong = false)
    {
        uint32_t lenStr = lengthLong ? *(uint32_t *)_curPos : *(uint16_t *)_curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 3000)
        {
            logger->warn("Encountered an unexpected string length. @{} in \"{}\"", _distance(), filename);
            _sendFailedSignal();
            return;
        }

        _skip(lenInt + lenStr);
    }

    /**
     * \brief      跳过“长度（2字节/4字节）+字符串内容”格式的字符串
     *
     * \param      s                   用于存储字符串的变量
     * \param      convertEncodeing    是否对读取的字符串进行转码
     * \param      lengthLong          长度是用4个字节（true）还是2个字节（false）表示
     */
    void _readPascalString(string &s, bool convertEncodeing = true,
                           bool lengthLong = false)
    {
        uint32_t lenStr = lengthLong ? *(uint32_t *)_curPos : *(uint16_t *)_curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 3000)
        {
            logger->warn("Encountered an unexpected string length. @{} in \"{}\"", _distance(), filename);
            _sendFailedSignal();
            return;
        }

        _skip(lenInt);
        s.assign((char *)_curPos, lenStr);
        _skip(lenStr);

        if (convertEncodeing && (_encodingConverter != nullptr))
        {
            fixEncoding(s);
        }
    }

    /**
     * \brief      _skipPascalString的一个特殊版本
     */
    inline void _skipDEString()
    {
        int16_t l[2];
        _readBytes(4, l);
        if (l[0] != 2656) // 0x60 0x0a int16_t
        {
            logger->warn("_skipDEString: Encountered an unexpected DE string. @{} in \"{}\"", _distance(), filename);
            _sendFailedSignal();
            _curPos -= 4;
            return;
        }
        _skip(l[1]);
    }

    /**
     * \brief      _skipPascalString的一个特殊版本
     */
    inline void _skipHDString()
    {
        int16_t l;
        _readBytes(2, &l);
        if (*(uint16_t *)_curPos != 2656) // 0x60 0x0a int16_t
        {
            logger->warn("_skipHDString: Encountered an unexpected HD string. @{} in \"{}\"", _distance(), filename);
            _sendFailedSignal();
            _curPos -= 2;
            return;
        }
        _skip(2 + l);
    }

    /**
     * \brief      _readPascalString的一个特殊版本
     *
     * \param      s                   用于存储读取结果的变量
     */
    inline void _readDEString(string &s)
    {
        uint16_t l;

        if (!(*(uint16_t *)_curPos == 2656)) // 0x60 0x0a
        {
            logger->warn("_readDEString: Encountered an unexpected DE string. @{} in \"{}\"", _distance(), filename);
            _sendFailedSignal();
            return;
        }

        _skip(2);
        _readBytes(2, &l);
        s.assign((char *)_curPos, l);
        _skip(l);
    }

    inline void _readHDString(string &s)
    {
        uint16_t l;
        _readBytes(2, &l);

        if (*(uint16_t *)_curPos != 2656) // 0x60 0x0a
        {
            logger->warn("_readHDString: Encountered an unexpected HD string. @{} in \"{}\"", _distance(), filename);
            _sendFailedSignal();
            _curPos -= 2;
            return;
        }

        _skip(2);
        s.assign((char *)_curPos, l);
        _skip(l);
    }

    void _skip(size_t n) { _curPos += n; } ///< Skip forward n bytes.

    void _analyze(); ///< 录像解析的主进程

    int _setVersionCode(); ///< 这个不是原始数据，是自己归纳出的一个版本识别特征码，后面便于判断版本

    /**
     * \brief      Compare two C-style strings or byte sequence.
     *
     * \param      s                   string1
     * \param      pattern             string2
     * \return     true                string1 == string2
     * \return     false               string1 != string2
     */
    inline bool _bytecmp(const void *s, const void *pattern, size_t n) const
    {
        return 0 == memcmp(s, pattern, n);
    }

    /**
     * \brief      用于检查当前位置的特征字节是否符合预期
     *
     * \param      pattern             特征字节
     */

    /**
     * \brief      用于检查当前位置的特征字节是否符合预期
     * 
     * \param      pattern             特征字节
     * \param      skip                检查完是否跳过这些字节
     * \return     true                验证通过
     * \return     false               验证失败
     */
    bool _expectBytes(const vector<uint8_t> &pattern, bool skip = true);

    /**
     * \brief      调用logger的功能打印出当前位置之后n个字节的16进制表示，用于调试
     *
     * \param      n                   要打印的字节数
     * \param      file                文件名
     * \param      line                行号
     */
    inline void _printHex(size_t n, string file, size_t line)
    {
        if (nullptr == logger)
            return;
        if (_remainBytes() <= n)
            n = _remainBytes();

        logger->logHex(n, _header.begin() + _distance(), _distance(), file, line);
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
    bool _findEncodingPattern(const char *pattern, std::string &mapName,
                              size_t patternLen);

    void _guessEncoding(); ///< 尝试推断录像文件中字符串的原始编码

    void _sendFailedSignal(bool fatal = false) {
        _failedSignal = true;
        if (fatal && logger) {
            message = logger->dumpStr();
        }
        status = fatal ? "fatal" : "warning";
    } ///< 标记解析失败的FLAG

    void _headerHDAnalyzer();
    void _headerDEAnalyzer();
    void _AIAnalyzer();
    void _replayAnalyzer();
    void _mapDataAnalyzer();
    void _findStartInfoStart();
    void _findTriggerInfoStart();
    void _findDisablesStart();
    void _findGameSettingsStart();
    void _findVictoryStart();
    void _findScenarioHeaderStart();
    void _scenarioHeaderAnalyzer();
    void _messagesAnalyzer();
    void _victorySettingsAnalyzer();
    void _gameSettingsAnalyzer();
    void _findInitialPlayersDataPos();
    void _startInfoAnalyzer();
    void _triggerInfoAnalyzer();
    void _lobbyAnalyzer();

    ifstream _f;             ///< 读取后的录像文件数据
    uintmax_t _bodySize;     ///< body部分的长度(bytes)
    vector<uint8_t> _body;   ///< 用于存储body数据
    vector<uint8_t> _header; ///< 用于存储解压缩后的header数据

    uint8_t *_curPos;    ///< 当前读取数据的指针
    uint8_t *_curStream; ///< 指向当前使用的数据流的底层数组的指针

    uint32_t _DD_AICount = 0; ///< \note used to skip AI section
    uint8_t *_startInfoPatternTrail;
    uint8_t _mapTileType = 0; ///< \note 7: DETile1; 9: DETile2; 4: Tile1; 2: TileLegacy. This value is size of structure.

    uint8_t *_startInfoPos = nullptr;
    uint8_t *_triggerInfoPos = nullptr;
    uint8_t *_gameSettingsPos = nullptr;
    uint8_t *_disablesStartPos = nullptr;
    uint8_t *_victoryStartPos = nullptr;
    uint8_t *_scenarioHeaderPos = nullptr;
    uint8_t *_messagesStartPos = nullptr;
    uint8_t *_lobbyStartPos = nullptr;

    EncodingConverter *_encodingConverter = nullptr;

    bool _failedSignal = false; ///< Indicate some previous procedure was failed
};