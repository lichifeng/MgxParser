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
#include "Analyzers/default/Analyzer.h"
#include "include/MgxParser.h"

using namespace std;

string MgxParser::parse(const string &recfile, int mapType)
{
    auto a = DefaultAnalyzer(recfile);

    try
    {
        a.run();
    }
    catch (const exception &e)
    {
        a.logger->fatal("Exception@_debugFlag#{}: {}", a.getDebugFlag(), e.what());
    }

    a.parseTime = a.logger->elapsed();
    a.message = a.logger->dumpStr();

    if (mapType != NO_MAP) {
        a.generateMap("testmap.png", 300, 150, mapType == HD_MAP);
    }

    return a.toJson();
}

string MgxParser::parse(const uint8_t *buf, size_t n, int mapType)
{
    auto a = DefaultAnalyzer(buf, n);

    try
    {
        a.run();
    }
    catch (const exception &e)
    {
        a.logger->fatal("Exception@_debugFlag#{}: {}", a.getDebugFlag(), e.what());
    }

    a.parseTime = a.logger->elapsed();
    a.message = a.logger->dumpStr();

    if (mapType != NO_MAP) {
        a.generateMap("testmap.png", 300, 150, mapType == HD_MAP);
    }

    return a.toJson();
}

extern "C"
{
    const char *MgxParser::pyparse(const char *recfile)
    {
        string j = MgxParser::parse(recfile).c_str();
        char* retStr = (char*)malloc(j.size() + 1);
        strcpy(retStr, j.c_str());
        return retStr;
    }
}