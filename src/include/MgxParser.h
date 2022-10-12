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

#include <string>

namespace MgxParser
{
    std::string parse(const uint8_t *, size_t);
    std::string parse(const std::string &);
    extern "C" const char* pyparse(const char*);
}

#endif