/**
 * \file       Logger.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-01
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#pragma once
#include "CompileConfig.h"
#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/stopwatch.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/ostream_sink.h"

using namespace std;

enum LoggerDest {
    CONSOLE,
    LOGSTRING,
    LOCALFILE
};

class Logger
{
private:
    shared_ptr<spdlog::logger>                  _logger = nullptr;
    string                                      _defaultName = "DefaultLogger";
    spdlog::stopwatch                           _sw;
    ostringstream                               _oss;

    void _debugLoggers() {
        vector<spdlog::sink_ptr> sinks;
        sinks.push_back(make_shared<spdlog::sinks::ostream_sink_mt>(_oss));
        sinks.push_back(make_shared<spdlog::sinks::stdout_color_sink_mt>());
        
        _logger = make_shared<spdlog::logger>(_defaultName, begin(sinks), end(sinks));
    }

    void _stringLogger() {
        auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(_oss);
        _logger = make_shared<spdlog::logger>(_defaultName, oss_sink);
    }

public:
    Logger() {
        if (DEBUG) {
            _debugLoggers();
        } else {
            _stringLogger();
        }

        setPattern();

        // ...
    }

    string dumpStr() {
        return _oss.str();
    }

    void setPattern(string pattern = "") {
        // _logger->set_formatter(std::unique_ptr<spdlog::formatter>(new spdlog::pattern_formatter("[%Y-%m-%d %T][%^%8l%$] %v")));
        if (pattern.length() > 0) {
            _logger->set_pattern(pattern);
        } else {
            _logger->set_pattern(
                "[%Y-%m-%d %T][%^%8l%$] %v"
            );
        }
    }

    template<typename... Args>
    inline string fmt(Args... args) {
        return fmt::format(args...);
    }

    double elapsed() {
        return _sw.elapsed().count() * 1000;
    }

    template<typename... Args>
    inline void info(Args... args) {
        return _logger->info(args...);
    }

    template<typename... Args>
    inline void warn(Args... args) {
        return _logger->warn(args...);
    }

    template<typename... Args>
    inline void fatal(Args... args) {
        return _logger->critical(args...);
    }

    inline void logHex(size_t n, vector<uint8_t>::iterator it, size_t offset, string file, size_t line) {
        return info(
            "[{:X:p:n} ] @Offset: {}, {}:{} ", 
            spdlog::to_hex(it, it + n),
            offset,
            file, 
            line
        );
    }
};

