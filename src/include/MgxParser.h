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

#ifndef __MGXPARSER__
#define __MGXPARSER__

#define NO_MAP 0
#define NORMAL_MAP 1
#define HD_MAP 2

#include <string>

namespace MgxParser
{
    std::string parse(const uint8_t *, size_t, int = 0, std::string = "minimap.png");
    std::string parse(const std::string &, int = 0, std::string = "minimap.png");
    extern "C" const char* pyparse(const char*);
}

#endif