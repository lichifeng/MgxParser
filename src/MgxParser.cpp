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

string _parse(DefaultAnalyzer &a, int mapType, string mapName, bool extractHB = false)
{
    try
    {
        a.run();
    }
    catch (const string &s)
    {
        a.logger->fatal("{}: {}", a.filename, s);
    }
    catch (const exception &e)
    {
        a.logger->fatal("Exception at Flag#{} in {}! {}", a.getDebugFlag(), a.filename, e.what());
    }
    catch (...)
    {
        a.logger->fatal("Exception at Flag#{} in {}!", a.getDebugFlag(), a.filename);
    }

    a.parseTime = a.logger->elapsed();
    a.message = a.logger->dumpStr();

    if (mapType != NO_MAP)
    {
        uint32_t w = HD_MAP ? 900 : 300;
        uint32_t h = HD_MAP ? 450 : 150;
        try
        {
            a.generateMap(mapName, w, h, mapType == HD_MAP);
        }
        catch (...)
        {
            a.logger->warn("Failed to generate a map file.");
        }
    }

    if (extractHB)
        a.extract("header.dat", "body.dat");

    return a.toJson();
}

string MgxParser::parse(const string &recfile, int mapType, string mapName, bool extractHB)
{
    auto a = DefaultAnalyzer(recfile);

    return _parse(a, mapType, mapName, extractHB);
}

string MgxParser::parse(const uint8_t *buf, size_t n, int mapType, string mapName, bool extractHB)
{
    auto a = DefaultAnalyzer(buf, n);

    return _parse(a, mapType, mapName, extractHB);
}

extern "C"
{
    /**
     * \brief      This function is a early try of python integration, not tested.
     *
     * \param      recfile             Path of record file
     * \return     const char*         JSON string contains parsed information
     */
    const char *MgxParser::pyparse(const char *recfile)
    {
        string j = MgxParser::parse(recfile).c_str();
        char *retStr = (char *)malloc(j.size() + 1);
        strcpy(retStr, j.c_str());
        return retStr;
    }
}