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

#define HEADER_INIT 5 * 1024 * 1024
#define BODY_MAX 100 * 1024 * 1024
#define HEADER_STRM 0
#define BODY_STRM 1
#define PrintHEX(n) _printHex(n, __FILE__, __LINE__)
#define FILE_INPUT 1
#define MEM_INPUT 2
#define ZIP_INPUT 3
#define EARLYMOVE_USED 5

#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

#include "EncodingConverter.h"
#include "Logger.h"
#include "../BaseAnalyzer.h"
#include "utils.h"
#include "bodyProcessors/helpers.h"
#include "Zipdecompress.h"

using namespace std;

/**
 * \brief      默认解析器，可以通过继续它来增加新的解析器。例如可以用来增加一个快速生成地图的版本，省略不必要的解析。
 *
 */
class DefaultAnalyzer : public BaseAnalyzer
{
public:
    ~DefaultAnalyzer()
    {
        if (nullptr != _zipinfo)
            delete _zipinfo;
        delete _encodingConverter;
    }

    DefaultAnalyzer(const string &inputFile) : path(inputFile) { createLogger(); }

    DefaultAnalyzer(const uint8_t *buff, size_t buffLen) : _inputType(MEM_INPUT), _b(buff)
    {
        filesize = buffLen;
    }

    void run();

    inline int getDebugFlag() { return _debugFlag; }

    void generateMap(const string path, uint32_t width = 300, uint32_t height = 150, bool hd = false);

    ///< Convert a string from record file encoding to specified output
    ///< encoding. Input string not necessarily a member of this class, so its
    ///< a public function.

    /**
     * \brief      将输入的字符串转换成设定的编码（一般是UTF-8）
     * \todo       正在考虑是否将编码转换的功能全部移到JSON输出前去，改用boost.locale库，Analyzer里只读原始编码的字符串？
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

        if (0 == rawEncoding.compare(outEncoding))
            return s;

        try
        {
            _encodingConverter->convert(s, s);
        }
        catch (const exception &e)
        {
            logger->warn("Encoding Exception@{}: {}", _debugFlag, e.what());
            _sendFailedSignal(true);
        }

        return s;
    }

    /**
     * \brief      Extract header&body streams into separate files
     *
     * \param      headerPath       filename of generated header file
     * \param      body             filename of generated body file
     */
    void extract(const string, const string) const;

    void createLogger()
    {
        if (!logger)
            logger = new Logger();
    }

    Logger *logger = nullptr;

    string path; ///< 录像的路径

protected:
    void _loadFile(); ///< 从文件中加载数据流

    bool _locateStreams(); ///< 对文件流进行处理，定位 header & body 的起始位置

    int _inflateRawHeader(); ///< 解压 header 数据

    /**
     * \brief      切换当前工作的数据流（header 或者 body）
     *
     * \param      stream              HEADER_STRM/BODY_STRM
     */
    inline void _switchStream(uint8_t stream = HEADER_STRM)
    {
        (HEADER_STRM == stream) ? _curStream = &_header
                                : _curStream = &_body;
        _curPos = _curStream->data();
    }

    inline size_t _distance() { return _curPos - _curStream->data(); } ///< 获取当前读取位置（相对于STREAM开头）

    inline size_t _remainBytes()
    {
        return (_curStream->size() >= _distance()) ? (_curStream->size() - _distance()) : 0;
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
            logger->warn("Encountered an unexpected string length[SKIP]. @{}, Flag:{} in \"{}\"", _distance(), _debugFlag, filename);
            _sendFailedSignal();
            return;
        }

        _skip(lenInt + lenStr);
    }

    /**
     * \brief      跳过“长度（2字节/4字节）+字符串内容”格式的字符串
     *
     * \param      s                   用于存储字符串的变量
     * \param      convertEncoding    是否对读取的字符串进行转码
     * \param      lengthLong          长度是用4个字节（true）还是2个字节（false）表示
     */
    void _readPascalString(string &s, bool convertEncoding = true,
                           bool lengthLong = false)
    {
        uint32_t lenStr = lengthLong ? *(uint32_t *)_curPos : *(uint16_t *)_curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 3000)
        {
            logger->warn("Encountered an unexpected string length[READ]. @{}, Flag:{} in \"{}\"", _distance(), _debugFlag, filename);
            _sendFailedSignal();
            return;
        }

        _skip(lenInt);
        s.assign((char *)_curPos, lenStr);
        _skip(lenStr);

        if (convertEncoding)
        {
            if (_encodingConverter != nullptr)
            {
                fixEncoding(s);
            }
            // else
            // {
            //     logger->warn("Use string reader before encoding is set. [READ]. @{}, Flag:{} in \"{}\"", _distance(), _debugFlag, filename);
            //     _sendFailedSignal();
            // }
        }
    }

    /**
     * \brief      _skipPascalString的一个特殊版本
     */
    inline void _skipDEString()
    {
        int16_t l[2];
        _readBytes(4, l);
        if (l[0] != 2656 || l[1] > _remainBytes()) // 0x60 0x0a int16_t
        {
            logger->warn("_skipDEString Exception! Length:{}, [0x60 0x0a]: {} . @{} in \"{}\"", l[1], l[0], _distance(), filename);
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
        if (*(uint16_t *)_curPos != 2656 || l > _remainBytes()) // 0x60 0x0a int16_t
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
            PrintHEX(4);
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

        logger->logHex(n, _curStream->begin() + _distance(), _distance(), file, line);
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

    void _sendFailedSignal(bool fatal = false)
    {
        _failedSignal = true;
        if (fatal && logger)
        {
            message = logger->dumpStr();
        }
        status = fatal ? "fatal" : "warning";
    } ///< 标记解析失败的FLAG

    void _headerHDAnalyzer(int debugFlag = 0);
    void _headerDEAnalyzer(int debugFlag = 0);
    void _AIAnalyzer(int debugFlag = 0);
    void _replayAnalyzer(int debugFlag = 0);
    void _mapDataAnalyzer(int debugFlag = 0);
    void _findStartInfoStart(int debugFlag = 0);
    void _findTriggerInfoStart(int debugFlag = 0);
    void _findDisablesStart(int debugFlag = 0);
    void _findGameSettingsStart(int debugFlag = 0);
    void _findVictoryStart(int debugFlag = 0);
    void _findScenarioHeaderStart(int debugFlag = 0);
    void _scenarioHeaderAnalyzer(int debugFlag = 0);
    void _messagesAnalyzer(int debugFlag = 0);
    void _victorySettingsAnalyzer(int debugFlag = 0);
    void _gameSettingsAnalyzer(int debugFlag = 0);
    void _findInitialPlayersDataPos(int debugFlag = 0);
    void _startInfoAnalyzer(int debugFlag = 0);
    void _triggerInfoAnalyzer(int debugFlag = 0);
    void _lobbyAnalyzer(int debugFlag = 0);
    void _readBodyCommands(int debugFlag = 0);
    void _readGameStart(int debugFlag = 0);

    // methods for parsing body data
    void _handleOpSync();
    void _handleOpViewlock();
    void _handleOpChat();
    void _handleOpCommand();

    void _handleAction();

    // Some additional jobs
    void _guessWinner(int);
    void _genRetroGuid(int);

    uint32_t ZLIB_CHUNK = 512 * 1024; ///< ZLIB解压时的参数。CHUNK is simply the buffer size for feeding data to and pulling data from the zlib routines. Larger buffer sizes would be more efficient, especially for inflate(). If the memory is available, buffers sizes on the order of 128K or 256K bytes should be used.
    ZipInfo *_zipinfo = nullptr;
    ifstream _f;             ///< 读取后的录像文件数据
    const uint8_t *_b;       ///< 以字节数组输入时的原始数组
    size_t _bodySize;        ///< body部分的长度(bytes)
    vector<uint8_t> _body;   ///< 用于存储body数据
    vector<uint8_t> _header; ///< 用于存储解压缩后的header数据

    const uint8_t *_curPos;      ///< 当前读取数据的指针
    vector<uint8_t> *_curStream; ///< 指向当前使用的数据流的底层数组的指针。 \todo 要把代码中所有的_header.data()替换成这个。

    uint32_t _DD_AICount = 0; ///< \note used to skip AI section
    const uint8_t *_startInfoPatternTrail;
    uint8_t _mapTileType = 0; ///< \note 7: DETile1; 9: DETile2; 4: Tile1; 2: TileLegacy. This value is size of structure.

    const uint8_t *_startInfoPos = nullptr;
    const uint8_t *_triggerInfoPos = nullptr;
    const uint8_t *_gameSettingsPos = nullptr;
    const uint8_t *_disablesStartPos = nullptr;
    const uint8_t *_victoryStartPos = nullptr;
    const uint8_t *_scenarioHeaderPos = nullptr;
    const uint8_t *_messagesStartPos = nullptr;
    const uint8_t *_lobbyStartPos = nullptr;

    EncodingConverter *_encodingConverter = nullptr;

    bool _failedSignal = false; ///< Indicate some previous procedure was failed
    int _debugFlag = 0;
    int _inputType = FILE_INPUT;

    const uint8_t *_earlyMoveCmd[EARLYMOVE_USED]; ///< 有时候用自定义地图时，各方面初始数据会非常类似，造成无法准确判断不同视角是否属于同一局录像。所以要从BODY里的命令中提取一条，加入GUID计算中，这样重复的可能性就少了很多。MOVE的动作是几乎每局录像都会有的。
    uint32_t _earlyMoveTime[EARLYMOVE_USED];
    int _earlyMoveCnt;
};