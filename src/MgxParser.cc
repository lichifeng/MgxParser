/***************************************************************
 * \file       MgxParser.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <string>
#include <utility>
#include "analyzers/default/analyzer.h"
#include "include/MgxParser.h"

std::string _parse(DefaultAnalyzer &a, int map_type, std::string &&map_name, bool extract = false) {
    try {
        a.Run();
    }
    catch (std::string &s) {
        a.logger_->Fatal("Df#{}@{}/{}: {}", a.status_.debug_flag_, a.Position(), a.TotalSize(), s);
    }
    catch (const std::exception &e) {
        a.logger_->Fatal("Df#{}@{}/{}: {}", a.status_.debug_flag_, a.Position(), a.TotalSize(), e.what());
    }
    catch (...) {
        a.logger_->Fatal("Df#{}@{}/{}: Unknown Exception!", a.status_.debug_flag_, a.Position(), a.TotalSize());
    }

    a.parse_time_ = a.logger_->Elapsed();

    if (map_type != NO_MAP) {
        uint32_t w = HD_MAP ? 900 : 300;
        uint32_t h = HD_MAP ? 450 : 150;
        try {
            a.DrawMap(map_name, w, h, map_type == HD_MAP);
        }
        catch (...) {
            a.logger_->Warn("Failed to generate a map file.");
        }
    }

    a.message_ = std::move(a.logger_->DumpStr());

    if (extract)
        a.Extract2Files("header.dat", "body.dat");

    return a.JsonOutput();
}

std::string MgxParser::parse(std::string input_path, int map_type, std::string map_name, bool extract_stream) {
    auto a = DefaultAnalyzer(std::move(input_path));
    return _parse(a, map_type, std::move(map_name), extract_stream);
}

std::string
MgxParser::parse(const uint8_t *input, size_t input_size, int map_type, std::string map_name, bool extract_stream) {
    auto a = DefaultAnalyzer(input, input_size);
    return _parse(a, map_type, std::move(map_name), extract_stream);
}

extern "C"
{
const char *MgxParser::pyparse(const char *input_path, int map_type, const char *map_name, bool extract_stream) {
    auto a = DefaultAnalyzer(input_path);
    std::string &&result = std::move(_parse(a, map_type, map_name, extract_stream));
    return result.c_str();
}
}