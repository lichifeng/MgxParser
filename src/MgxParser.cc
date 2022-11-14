/***************************************************************
 * \file       MgxParser.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <string>
#include <utility>
#include "include/MgxParser.h"
#include "analyzers/default/analyzer.h"

std::string _parse(DefaultAnalyzer &&a, int map_type, FILE *map_dest, const std::string &map_name, int map_width,
                   int map_height, bool extract = false, const std::string &header_path = "",
                   const std::string &body_path = "", bool full_parse = true) {
    if (full_parse) {
        try {
            a.Run();
        } catch (std::string &s) {
            a.logger_->Fatal("Df#{}@{}/{}: {}", a.status_.debug_flag_, a.Position(), a.TotalSize(), s);
        } catch (const std::exception &e) {
            a.logger_->Fatal("Df#{}@{}/{}: {}", a.status_.debug_flag_, a.Position(), a.TotalSize(), e.what());
        } catch (...) {
            a.logger_->Fatal("Df#{}@{}/{}: Unknown Exception!", a.status_.debug_flag_, a.Position(), a.TotalSize());
        }
        a.parse_time_ = a.logger_->Elapsed();
    }

    if (a.MapReady() && map_type != NO_MAP) {
        if (map_width < 0)
            map_width = 300;
        if (map_height < 0)
            map_height = 150;
        try {
            if (map_dest) {
                a.DrawMap(map_dest, map_width, map_height, map_type == HD_MAP);
            }
            if (!map_name.empty()) {
                a.DrawMap(map_name, map_width, map_height, map_type == HD_MAP);
            }
        } catch (...) {
            a.logger_->Warn("Failed to generate a map file.");
        }
    }

    if (a.StreamReady() && (extract || !header_path.empty() || !body_path.empty())) {
        a.Extract2Files(header_path, body_path);
    }

    a.message_ = std::move(a.logger_->DumpStr());

    return a.JsonOutput();
}

std::string MgxParser::parse(Settings &settings) {
    if (settings.input_stream && settings.input_size > 0) {
        return _parse(DefaultAnalyzer(settings.input_stream, settings.input_size, settings.input_path),
                      settings.map_type, settings.map_dest, settings.map_name, settings.map_width, settings.map_height,
                      settings.extract_stream, settings.header_path, settings.body_path, settings.full_parse);
    } else {
        return _parse(DefaultAnalyzer(settings.input_path), settings.map_type, settings.map_dest, settings.map_name,
                      settings.map_width, settings.map_height, settings.extract_stream, settings.header_path,
                      settings.body_path, settings.full_parse);
    }
}

extern "C" {
const char *MgxParser::pyparse(const char *input_path, int map_type, const char *map_name, bool extract_stream) {
    std::string &&result = std::move(_parse(DefaultAnalyzer(input_path), map_type, NULL, map_name, 300, 150));
    return result.c_str();
}
}