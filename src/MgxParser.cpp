/**
 * \file       MgxParser.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-09
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include <string>
#include <utility>
#include "analyzers/default/analyzer.h"
#include "include/MgxParser.h"

using namespace std;

string _parse(DefaultAnalyzer &a, int mapType, string mapName, bool extractHB = false) {
    try {
        a.run();
    }
    catch (std::string &s) {
        a.logger_->fatal("{}: {}", a.input_filename_, s);
    }
    catch (const exception &e) {
        std::cout << e.what() << endl;
    }
    catch (...) {
        a.logger_->fatal("Exception at Flag#{} in {}!", a.getDebugFlag(), a.input_filename_);
    }

    a.parseTime = a.logger_->elapsed();
    a.message = a.logger_->dumpStr();

    if (mapType != NO_MAP) {
        uint32_t w = HD_MAP ? 900 : 300;
        uint32_t h = HD_MAP ? 450 : 150;
        try {
            a.generateMap(mapName, w, h, mapType == HD_MAP);
        }
        catch (...) {
            a.logger_->warn("Failed to generate a map file.");
        }
    }

    if (extractHB)
        a.Extract2Files("header.dat", "body.dat");

    return a.toJson();
}

string MgxParser::parse(std::string inputpath, int maptype, std::string mapname, bool extract_stream) {
    auto a = DefaultAnalyzer(inputpath);

    return _parse(a, maptype, mapname, extract_stream);
}

string MgxParser::parse(const uint8_t *buf, size_t n, int mapType, string mapName, bool extractHB) {
    auto a = DefaultAnalyzer(buf, n);

    return _parse(a, mapType, mapName, extractHB);
}

extern "C"
{
const char *MgxParser::pyparse(const char *recfile, int maptype, const char *mapname, bool extract_stream) {
    auto a = DefaultAnalyzer(recfile);
    std::string &&ret = std::move(_parse(a, maptype, mapname, extract_stream));
    return ret.c_str();
}
}