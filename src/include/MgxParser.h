/**
 * \file       MgxParser.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-09
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#ifndef MGXPARSER_H_
#define MGXPARSER_H_

#define NO_MAP 0
#define NORMAL_MAP 1
#define HD_MAP 2

#include <string>

namespace MgxParser
{
    std::string parse(const uint8_t *, size_t, int = 0, std::string = "minimap.png", bool extractHD = false);
    std::string parse(std::string inputpath, int maptype = 0, std::string mapname = "minimap.png", bool extract_stream = false);
    extern "C" const char* pyparse(const char *recfile, int maptype, const char* mapname, bool extract_stream = false);
}

#endif