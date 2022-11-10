/***************************************************************
 * \file       logger.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#ifndef MGXPARSER_LOGGER_H_
#define MGXPARSER_LOGGER_H_

#include "compile_config.h"
#include <string>
#include <sstream>
#include <vector>

#include "spdlog/spdlog.h"
#include "spdlog/sinks/stdout_color_sinks.h"
#include "spdlog/stopwatch.h"
#include "spdlog/fmt/bin_to_hex.h"
#include "spdlog/fmt/fmt.h"
#include "spdlog/sinks/ostream_sink.h"

class Logger {
private:
    std::shared_ptr<spdlog::logger> logger_ = nullptr;
    std::string default_name_ = "DefaultLogger";
    spdlog::stopwatch sw_;
    std::ostringstream oss_;

    void DebugLoggers() {
        std::vector<spdlog::sink_ptr> sinks;
        sinks.push_back(std::make_shared<spdlog::sinks::ostream_sink_mt>(oss_));
        sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());

        logger_ = std::make_shared<spdlog::logger>(default_name_, begin(sinks), end(sinks));
    }

    void StringLogger() {
        auto oss_sink = std::make_shared<spdlog::sinks::ostream_sink_mt>(oss_);
        logger_ = std::make_shared<spdlog::logger>(default_name_, oss_sink);
    }

public:
    Logger() {
        if (DEBUG) {
            DebugLoggers();
        } else {
            StringLogger();
        }

        SetPattern();
    }

    std::string DumpStr() {
        return oss_.str();
    }

    void SetPattern(const std::string& pattern = "") {
        if (pattern.length() > 0) {
            logger_->set_pattern(pattern);
        } else {
            logger_->set_pattern(
                    "[%Y-%m-%d %T][%^%8l%$] %v"
            );
        }
    }

    template<typename... Args>
    inline std::string Fmt(Args... args) {
        return fmt::format(args...);
    }

    double Elapsed() {
        return sw_.elapsed().count() * 1000;
    }

    template<typename... Args>
    inline void Info(Args... args) {
        logger_->info(args...);
    }

    template<typename... Args>
    inline void Warn(Args... args) {
        logger_->warn(args...);
    }

    template<typename... Args>
    inline void Fatal(Args... args) {
        logger_->critical(args...);
    }

    inline void
    LogHex(size_t n, std::vector<uint8_t>::iterator it, std::size_t offset, std::string file, std::size_t line) {
        return Info(
                "[{:X:p:n} ] @Offset: {}, {}:{} ",
                spdlog::to_hex(it, it + n),
                offset,
                file,
                line
        );
    }
};

#endif //MGXPARSER_LOGGER_H_
