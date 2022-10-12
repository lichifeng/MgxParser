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

#include <string>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/stopwatch.h"
#include "spdlog/fmt/bin_to_hex.h"

// Plan:
//   auto logger = Logger(); // auto detect debug/release
//   auto logger = Logger(CONSOLE);
//   auto logger = Logger(STREAM);
//   auto logger = Logger(FILE);
//   logger.elapsed(); // return elapsed time
//   logger.info("xxxxxxx");
//   logger.warn("xxxxxxx");
//   logger.fatal("XXXXXX"); // this stops analyzing process and return result

using namespace std;

enum LoggerDest {
    CONSOLE,
    STREAM,
    LOCALFILE
};

class Logger
{
private:
    shared_ptr<spdlog::logger> _logger = nullptr;
    string                     _defaultName = "DefaultLogger";
    spdlog::stopwatch               _sw;

    void _setLogger(LoggerDest ld) {
        switch (ld)
        {
        case CONSOLE:
            _logger = spdlog::stdout_color_mt(_defaultName);
            break;
        case STREAM:
            break;
        case LOCALFILE:
            // try { ... } catch () { ... }
            break;
        }
    }

public:
    Logger(LoggerDest ld) {
        _setLogger(CONSOLE);

        Logger();
    }

    Logger() {
        if (!_logger)
            _setLogger(CONSOLE);
        setPattern();

        // ...
    }

    void setPattern(string pattern = "") {
        if (pattern.length() > 0) {
            _logger->set_pattern(pattern);
        } else {
            _logger->set_pattern(
                "[%Y-%m-%d %T][%^%8l%$] %v"
            );
        }
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

