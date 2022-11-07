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
#ifndef MGXPARSER_DEFAULTANALYZER_H_
#define MGXPARSER_DEFAULTANALYZER_H_

#define RECBYTE uint8_t
#define MIN_SIZE (100 * 1024)

#define HEADER_STRM 0
#define BODY_STRM 1
#define PrintHEX(n) _printHex1(n, __FILE__, __LINE__)
#define EARLYMOVE_USED 5

#include "compile_config.h"

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
#include "Logger.h"
#include "utils.h"

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
            throw std::string("Failed to load file.");
    }

    DefaultAnalyzer(const uint8_t *input_buffer, size_t bufferlen, const std::string filename = "")
            : input_cursor_(input_buffer), input_size_(bufferlen), cursor_(combined_stream_) {
        SharedInit();

        input_filename_ = filename.empty() ? "<memory stream>" : filename;
        if (input_size_ > MIN_SIZE)
            status_.input_loaded_ = true;
        else
            throw std::string("Invalid input size.");
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

    // 第三阶段：开始解析
    std::size_t version_end_ = 0;
    std::size_t ai_start_ = 0;
    std::size_t replay_start_ = 0;
    std::size_t map_start_ = 0;
    std::size_t initinfo_start_ = 0;
    std::size_t trigger_start_ = 0;
    std::size_t gamesettings_start_ = 0;
    std::size_t disabledtechs_start_ = 0;
    std::size_t victory_start_ = 0;
    std::size_t scenario_start_ = 0;
    std::size_t message_start_ = 0;
    std::size_t lobby_start_ = 0;

    void Run();

    void Extract2Files(const string &header_path, const string &body_path);

    void DrawMap(const string &path, uint32_t width = 300, uint32_t height = 150, bool hd = false);

    ~DefaultAnalyzer() = default;


    string toJson();


    inline int getDebugFlag() { return _debugFlag; }


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
    uint8_t maptile_type_ = 0; ///< \note 7: DETile1; 9: DETile2; 4: Tile1; 2: TileLegacy. This value is size of structure.
    uint32_t triggerstart_search_range = 19; ///< 查找triggerinfo位置时的参数，较早版本有0和1，DE中一般为11，如果找不到可以考虑放大范围试试
    const uint8_t *initinfo_searchpattern_trail_ = nullptr;
    uint16_t trailbyte_num_ = 5;               ///< 设定用于startinfo中玩家信息搜索时的特征字节长度，影响速度
    uint32_t easyskip_base_ = 35100;         ///< 在startinfo中搜索时可以放心跳过的字节长度
    const uint8_t *earlymove_cmd_[EARLYMOVE_USED]; ///< 有时候用自定义地图时，各方面初始数据会非常类似，造成无法准确判断不同视角是否属于同一局录像。所以要从BODY里的命令中提取一条，加入GUID计算中，这样重复的可能性就少了很多。MOVE的动作是几乎每局录像都会有的。
    uint32_t earlymove_time_[EARLYMOVE_USED];
    int earlymove_count_;

    void Analyze(); ///< 录像解析的主进程

    void DetectVersion();

    void AnalyzeDEHeader(int debugFlag = 0);

    void AnalyzeHDHeader(int debugFlag = 0);

    void AnalyzeAi(int debugFlag = 0);

    void AnalyzeReplay(int debug_flag = 0);

    void AnalyzeMap(int debug_flag = 0);

    void FindTrigger(int debug_flag = 0);

    void FindDisabledTechs(int debug_flag = 0);

    void FindGameSettings(int debug_flag = 0);

    void AnalyzeVictory(int debug_flag = 0);

    void AnalyzeScenario(int debug_flag = 0, bool brutal = false, float lower_limit = 1.35, float upper_limit = 1.55);

    void AnalyzeMessages(int debug_flag = 0);

    bool FindEncodingPattern(const char *pattern, std::string &map_name, size_t pattern_len);

    void DetectEncoding();

    void AnalyzeGameSettings(int debug_flag = 0);

    void FindInitialDataPosition(int debug_flag = 0);

    void SkipTriggers(int debug_flag = 0);

    void AnalyzeInitialData(int debug_flag = 0);

    void AnalyzeLobby(int debug_flag = 0);

    void ReadBodyCommands(int debug_flag = 0);

    void ReadGameStart(int debug_flag = 0);

    // methods for parsing body data
    void HandleSync();

    void HandleViewlock();

    void HandleChat();

    void HandleCommand();

    void HandleAction();

    // Some additional jobs
    void JudgeWinner(int);

    void CalcRetroGuid(int);


    vector<uint8_t> body_;
    vector<uint8_t> header_;

    std::string status_old_ = "good";


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

    void _sendExceptionSignal(bool throwException = false, string msg = "") {
        _failedSignal = true;
        status_old_ = throwException ? "Aborted" : "Warning";
        if (throwException) {
            if (logger_)
                message = logger_->dumpStr();
            throw msg;
        }
    } ///< 标记解析失败的FLAG

    const uint8_t *_curPos;      ///< 当前读取数据的指针
    vector<uint8_t> *_curStream; ///< 指向当前使用的数据流的底层数组的指针。 \todo 要把代码中所有的_header.data()替换成这个。

    uint32_t _DD_AICount = 0; ///< \note used to skip AI section

    const uint8_t *_startInfoPos = nullptr;
    const uint8_t *_lobbyStartPos = nullptr;

    bool _failedSignal = false; ///< Indicate some previous procedure was failed
    int _debugFlag = 0;
};

#endif //MGXPARSER_DEFAULTANALYZER_H_