/**
 * \file       analyzer.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-10-02
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#pragma once

#define RECBYTE uint8_t
#define MIN_SIZE (100 * 1024)

#define HEADER_STRM 0
#define BODY_STRM 1
#define PrintHEX(n) _printHex1(n, __FILE__, __LINE__)
#define EARLYMOVE_USED 5

#include "CompileConfig.h"

#include <array>
#include <cstddef>
#include <cstring>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <vector>

#include "status.h"
#include "cursor.h"
#include "DataModels/DataModel.h"
#include "encoding_converter.h"
#include "Logger.h"
#include "utils.h"
#include "bodyProcessors/helpers.h"

using namespace std;

/**
 * \brief      默认解析器，可以通过继承它来增加新的解析器。例如可以用来增加一个快速生成地图的版本，省略不必要的解析。
 *
 */
class DefaultAnalyzer : public DataModel // \todo 这个继承的设计并不好，反而始类之间存在牵绊，正在重构。
{
public:
    // 第一阶段：初始化，读取输入并处理。
    // 输入可以是一个文件（名），或是一个字节数组（的指针）。
    // 这个阶段的目的，是把文件或是字节数组的输入统一到一个变量（input_cursor_）上。
    // 初始化logger_也在这个阶段完成，因为读取文件阶段也可能有需要记录的事件。
    // 在这个阶段发生任何问题，应当返回status:invalid，代表没有进入有意义的分析阶段，
    // 连是否是一个录像文件都无法判断。

    Status status_;
    std::string inputpath_;
    std::string input_filename_ = "<no file>";
    size_t input_size_ = 0;
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<DataModel> record_; // 存放了与录像本身相关的所有信息。与解析过程相关的情况存放在这个类里。

    DefaultAnalyzer(const std::string &inputpath)
            : inputpath_(inputpath), cursor_(combined_stream_) {
        SharedInit();

        if (LoadFile())
            status_.input_loaded_ = true;
        else
            throw "Unexpected error in file loading process.";
    }

    DefaultAnalyzer(const uint8_t *input_buffer, size_t bufferlen, const std::string filename = "")
            : input_cursor_(input_buffer), input_size_(bufferlen), cursor_(combined_stream_) {
        SharedInit();

        input_filename_ = filename.empty() ? "<memory stream>" : filename;
        if (input_size_ > MIN_SIZE)
            status_.input_loaded_ = true;
        else
            throw "Size of input stream is too small.";
    }

    // 第一阶段结束，自以往后，都只需要操作input_cursor_

    // 第二阶段：获取header和body(合并到combined_stream_)。
    // 这个阶段首先是判断是否为ZIP文件，如果是，则解压获得原始录像文件。
    // 然后判断录像文件是否是header长度信息，如果没有，则查找确定，
    // 如果有，则再次解压，获得header_。body_只要复制就能获得。
    // 其实可以不用复制，但是为了操作的统一，还是这样做了。
    bool ExtractStreams();
    std::size_t header_start_ = 0;
    std::size_t body_start_ = 0;
    // 第二阶段结束

    // 第三阶段：开始读需要连续不能出错的部分





    ~DefaultAnalyzer() {
        delete _encodingConverter;
    }

    string toJson();

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
    inline string &fixEncoding(string &s) {
        if (0 == s.size())
            return s;

        if (0 == rawEncoding.compare(outEncoding))
            return s;

        try {
            if (nullptr != _encodingConverter)
                _encodingConverter->convert(s, s);
        }
        catch (const exception &e) {
            if (!_encodingError)
                _encodingError = true;
            _sendExceptionSignal();
        }

        return s;
    }

    /**
     * \brief      Extract header&body streams into separate files
     *
     * \param      headerPath       filename of generated header file
     * \param      body             filename of generated body file
     */
    void extract(const string &headerPath, const string &bodyPath) const;

protected:
    // 第一阶段
    std::ifstream input_file_;
    std::vector<RECBYTE> input_stream_;
    const RECBYTE *input_cursor_ = nullptr;

    inline void SharedInit() {
        BindLogger();
        BindRecord();
    }

    inline void BindLogger() {
        if (!logger_)
            logger_ = std::make_unique<Logger>();
    }

    inline void BindRecord() {
        if (!record_)
            record_ = std::make_unique<DataModel>();
    }

    bool LoadFile();
    // 第一阶段结束

    // 第二阶段
    vector<uint8_t> combined_stream_;
    RecCursor cursor_;
    // 第二阶段结束

    // 第三阶段



    vector<uint8_t> body_;
    vector<uint8_t> header_;

    std::string status_old_ = "good";

    size_t _bodySize;        ///< body部分的长度(bytes)



    /**
     * \brief      切换当前工作的数据流（header 或者 body）
     *
     * \param      stream              HEADER_STRM/BODY_STRM
     */
    inline void _switchStream(uint8_t stream = HEADER_STRM) {
        (HEADER_STRM == stream) ? _curStream = &header_
                                : _curStream = &body_;
        _curPos = _curStream->data();
    }

    inline size_t _distance() { return _curPos - _curStream->data(); } ///< 获取当前读取位置（相对于STREAM开头）

    inline size_t _remainBytes() {
        return (_curStream->size() >= _distance()) ? (_curStream->size() - _distance()) : 0;
    } ///< 获取当前位置之后剩余的字节数

    /**
     * \brief      将当前位置往后 n 个字节的数据存储到一个变量上
     *
     * \param      n                   往后读取的字节数
     * \param      dest                指向目标变量的指针
     */
    inline void _readBytes(size_t n, void *dest) {
        memcpy(dest, _curPos, n);
        _curPos += n;
    }

    /**
     * \brief      跳过“长度（2字节/4字节）+字符串内容”格式的字符串
     *
     * \param      lengthLong          长度是用4个字节（true）还是2个字节（false）表示
     */
    inline void _skipPascalString(bool lengthLong = false) {
        uint32_t lenStr = lengthLong ? *(uint32_t *) _curPos : *(uint16_t *) _curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 3000) {
            logger_->warn(
                    "Encountered an unexpected string length[_skipPascalString]. @{} / {}, Flag:{} in \"{}\"",
                    _distance(), _curStream->size(), _debugFlag, input_filename_);
            _sendExceptionSignal();
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
                           bool lengthLong = false) {
        uint32_t lenStr = lengthLong ? *(uint32_t *) _curPos : *(uint16_t *) _curPos;
        uint32_t lenInt = lengthLong ? 4 : 2;

        if (lenStr > 3000) {
            logger_->warn("Encountered an unexpected string length[READ]. @{}, Flag:{} in \"{}\"", _distance(),
                          _debugFlag, input_filename_);
            _sendExceptionSignal();
            _skip(lenInt);
            return;
        }

        _skip(lenInt);
        s.assign((char *) _curPos, lenStr);
        _skip(lenStr);

        if (convertEncoding)
            fixEncoding(s);
    }

    /**
     * \brief      _skipPascalString的一个特殊版本
     */
    inline void _skipDEString() {
        int16_t l[2];
        _readBytes(4, l);
        if (l[0] != 2656 || l[1] > _remainBytes()) // 0x60 0x0a int16_t
        {
            logger_->warn("_skipDEString Exception! Length:{}, [0x60 0x0a]: {} . @{} in \"{}\"", l[1], l[0],
                          _distance(), input_filename_);
            _sendExceptionSignal();
            _curPos -= 4;
            return;
        }
        _skip(l[1]);
    }

    /**
     * \brief      _skipPascalString的一个特殊版本
     */
    inline void _skipHDString() {
        int16_t l;
        _readBytes(2, &l);
        if (*(uint16_t *) _curPos != 2656 || l > _remainBytes()) // 0x60 0x0a int16_t
        {
            logger_->warn("_skipHDString: Encountered an unexpected HD string. @{} in \"{}\"", _distance(),
                          input_filename_);
            _sendExceptionSignal();
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
    inline void _readDEString(string &s) {
        uint16_t l;

        if (*(uint16_t *) _curPos != 2656) // 0x60 0x0a
        {
            logger_->warn("_readDEString: Encountered an unexpected DE string. @{} in \"{}\"", _distance(),
                          input_filename_);
            PrintHEX(4);
            _sendExceptionSignal();
            return;
        }

        _skip(2);
        _readBytes(2, &l);
        s.assign((char *) _curPos, l);
        _skip(l);
    }

    inline void _readHDString(string &s) {
        uint16_t l;
        _readBytes(2, &l);

        if (*(uint16_t *) _curPos != 2656) // 0x60 0x0a
        {
            logger_->warn("_readHDString: Encountered an unexpected HD string. @{} in \"{}\"", _distance(),
                          input_filename_);
            _sendExceptionSignal();
            _curPos -= 2;
            return;
        }

        _skip(2);
        s.assign((char *) _curPos, l);
        _skip(l);
    }

    inline void _skip(size_t n) // \todo n could be negtive too?
    {
        if (_curPos - _curStream->data() + n > _curStream->size()) {
            _sendExceptionSignal(
                    true,
                    logger_->fmt("Trying to escape current stream! Pos:{}", _distance()));
        } else {
            _curPos += n;
        }
    } ///< Skip forward n bytes. A check is deployed to avoid segment fault.

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
    inline bool _bytecmp(const void *s, const void *pattern, size_t n) const {
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
    inline void _printHex1(size_t n, string file, size_t line) {
        if (nullptr == logger_)
            return;
        if (_remainBytes() <= n)
            n = _remainBytes();

        logger_->logHex(n, _curStream->begin() + _distance(), _distance(), file, line);
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

    void _sendExceptionSignal(bool throwException = false, string msg = "") {
        _failedSignal = true;
        status_old_ = throwException ? "Aborted" : "Warning";
        if (throwException) {
            if (logger_)
                message = logger_->dumpStr();
            throw msg;
        }
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

    void
    _findScenarioHeaderStart(int debugFlag = 0, bool brutal = false, float lowerLimit = 1.35, float upperLimit = 1.55);

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
    bool _encodingError = false;

    const uint8_t *_earlyMoveCmd[EARLYMOVE_USED]; ///< 有时候用自定义地图时，各方面初始数据会非常类似，造成无法准确判断不同视角是否属于同一局录像。所以要从BODY里的命令中提取一条，加入GUID计算中，这样重复的可能性就少了很多。MOVE的动作是几乎每局录像都会有的。
    uint32_t _earlyMoveTime[EARLYMOVE_USED];
    int _earlyMoveCnt;
};