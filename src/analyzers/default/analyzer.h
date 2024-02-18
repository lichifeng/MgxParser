/***************************************************************
 * \file       analyzer.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_DEFAULTANALYZER_H_
#define MGXPARSER_DEFAULTANALYZER_H_

#define RECBYTE uint8_t
#define MIN_INPUT_SIZE (30 * 1024)
#define EARLYMOVE_USED 5

#include "compile_config.h"
#include <array>
#include <cstddef>
#include <cstring>
#include <cstdio>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <utility>
#include <vector>
#include <filesystem>

#include "status.h"
#include "cursor.h"
#include "record.h"
#include "logger.h"
#include "include/MgxParser.h"

/**
 * An analyzer loads record data and tries to extract useful information from it.
 * This DefaultAnalyzer accepts a file path or byte buffer as input, provides methods to
 * export reults as json string, generate mini map and extract header&body data into files.
 */
class DefaultAnalyzer : public Record {
public:
    // 第一阶段：初始化，读取输入并处理。
    // 输入可以是一个文件（名），或是一个字节数组（的指针）。
    // 这个阶段的目的，是把文件或是字节数组的输入统一到一个变量（input_cursor_）上。
    Status status_;
    std::string inputpath_;
    std::string input_filename_;
    size_t input_size_ = 0;
    std::unique_ptr<Logger> logger_;
    std::unique_ptr<Record> record_; // 存放了与录像本身相关的所有信息。DefaultAnalyzer是与解析过程相关的成员。
    bool calc_md5_ = true;
    MgxParser::MapType map_type_ = MgxParser::NO_MAP;
    std::string unzip_;
    char **unzip_buffer_ = nullptr;
    std::size_t *unzip_size_ptr_ = nullptr;

    DefaultAnalyzer(std::string input_path)
            : inputpath_(std::move(input_path)), cursor_(combined_stream_) {
        SharedInit();

        if (LoadFile())
            status_.input_loaded_ = true;
    }

    DefaultAnalyzer(const uint8_t *input_buffer, size_t bufferlen, const std::string input_path = "")
            : input_cursor_(input_buffer), input_size_(bufferlen), cursor_(combined_stream_), inputpath_(std::move(input_path)) {
        SharedInit();

        auto inputpath = std::filesystem::path(inputpath_);
        auto filename = inputpath.filename().generic_string();
        input_filename_ = filename.empty() ? "<memory stream>" : filename;
        if (input_size_ > MIN_INPUT_SIZE)
            status_.input_loaded_ = true;
        else
            Message("Invalid input size.");
    }

    ~DefaultAnalyzer() = default;

    // 第二阶段：获取header和body(合并到combined_stream_)。
    // 这个阶段首先是判断是否为ZIP文件，如果是，则解压获得原始录像文件。
    // 然后判断录像文件是否是header长度信息，如果没有，则查找确定，
    // 如果有，则再次解压，获得header_。body_只要复制就能获得。
    // 其实可以不用复制，但是为了操作的统一，还是这样做了。
    // 从这个阶段往后，应该只需要操作cursor就可以了。
    bool ExtractStreams();

    std::size_t header_start_ = 0;
    std::size_t body_start_ = 0;
    // 第二阶段结束

    // 第三阶段：解析
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

    /**
     * Run the parsing process. Need to be called manually after DefaultAnalyzer was initialized with valid input.
     */
    void Run();


    /**
     * Extract header&body data into separator files
     * @param header_path Path to header data
     * @param body_path   Path to body data
     */
    void Extract2Files(const std::string &header_path, const std::string &body_path);

    /**
     * Generate a mini map for this game
     * @param save_path Path to saved image. Generated image is in .png format.
     * @param width     Width of generated image
     * @param height    Height of generated image
     */
    void DrawMap(const std::string &save_path, uint32_t width = 300, uint32_t height = 150);
    
    /**
     * Generate a mini map for this game
     * @param dest      A FILE* handler
     * @param width     Width of generated image
     * @param height    Height of generated image
     */
    void DrawMap(FILE *dest, uint32_t width = 300, uint32_t height = 150);

    /**
     * Generate a base64 encoded string of mini map for this game
     * @param width     Width of generated image
     * @param height    Height of generated image
     * @return          A base64 encoded string of the map image
     */
    std::string DrawMap(uint32_t width = 300, uint32_t height = 150);

    /**
     * Translate raw numberic Info into readable terms.
     * @param l C++ map contains localized strings
     * @param i Raw data as key of may
     * @param d Default string if not found
     * @return
     */
    static std::string Translate(const std::map<uint32_t, std::string> &l, uint32_t i, std::string d = "-") {
        if (!(l.find(i) == l.end())) {
            return l.at(i);
        } else {
            return d;
        }
    }

    /**
     * Serialize record information into a json string
     * @param indent Indentation of output JSON string. Default is -1.
     * 
     * @return C++ string. Need to be parsed.
     */
    std::string JsonOutput(int indent = -1);

    /**
     * Append a message to output result.
     */
    inline DefaultAnalyzer &Message(std::string&& s) {
        message_.append(s);
        return *this;
    }

    inline std::size_t Position() { return cursor_(); }
    inline std::size_t TotalSize() { return cursor_.RawStream().size(); }
    inline bool StreamReady() { return status_.stream_extracted_; }
    inline bool MapReady() { return status_.mapdata_found_; }

protected:
    // 第一阶段
    std::ifstream input_file_;
    std::vector<RECBYTE> input_stream_;
    const RECBYTE *input_cursor_ = nullptr;

    /**
     * Initialization steps shared among construct methods
     */
    inline void SharedInit() {
        BindLogger();
        BindRecord();
    }

    inline void BindLogger() {
        if (!logger_)
            logger_ = std::make_unique<Logger>();
        logger_->SetPattern("[%^%8l%$]%v");
    }

    inline void BindRecord() {
        if (!record_)
            record_ = std::make_unique<Record>();
    }

    bool LoadFile();
    // 第一阶段结束

    // 第二阶段
    std::vector<uint8_t> combined_stream_;
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
    uint32_t dd_ai_count_ = 0; ///< \note used to skip AI section

    void Analyze();

    void Analyze2Map();

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

    void HandleAction(uint32_t &cmdlen);

    /**
     * Guess the winner team. Only for fun, not accurate.
     * @param debug_flag
     */
    void JudgeWinner(int);

    void CalcRetroGuid(int);

    static std::string CalcFileMd5(const uint8_t *, std::size_t);

    template<typename T, size_t N = sizeof(T)>
    bool FindEncodingPattern(const T (&pattern)[N]) {
        size_t pos, pos_end;

        if (std::string::npos != (pos = instructions.find((const char *) pattern, 0, N))) {
            pos_end = instructions.find('\n', pos + N);
            if (std::string::npos != pos_end)
                embeded_mapname_ = instructions.substr(pos + N, pos_end - pos - N);
            return true;
        }
        return false;
    }
};

#endif //MGXPARSER_DEFAULTANALYZER_H_