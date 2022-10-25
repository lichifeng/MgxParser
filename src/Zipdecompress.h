/**
 * \file       Zipdecompress.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-25
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include "zlib.h"

using namespace std;

struct ZipInfo
{
    int status = 0;
    string filename;
    vector<uint8_t> outBuffer;
    uintmax_t rawSize;
};
void fetchFromZip(const uint8_t *, ZipInfo *, int = -MAX_WBITS);