/**
 * \file       utils.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      Some utility for minor jobs
 * \version    0.1
 * \date       2022-09-24
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#pragma once

#include <string>
#include <algorithm>
#include <vector>
#include <array>

namespace patterns {
    vector<uint8_t> HDseparator = {0xa3, 0x5f, 0x02, 0x00};
    vector<uint8_t> HDStringSeparator = {0x60, 0x0a};
    vector<uint8_t> AIdataUnknown = {0x08, 0x00};
    vector<uint8_t> ZEROs_4096(4096, 0x00);
    vector<uint8_t> FFs_500(500, 0xff);
    vector<uint8_t> AIDirtyFix = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27};
}


constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

/**
 * \brief      Use to convert bytes to hex string, mainly used to generate guid
 * in HD versions.
 * 
 * \param      data                TEXT
 * \param      len                 TEXT
 * \param      skip                TEXT
 * \return     std::string         TEXT
 */
std::string hexStr(unsigned char*& data, int len, bool skip = false)
{
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  if (skip) data += len;
  return s;
}

template<typename T>
T findPosition(T haystackBeg, T haystackEnd, T needleBeg, T needleEnd)
{
    return std::search(
        haystackBeg, haystackEnd,
        std::boyer_moore_searcher(
            needleBeg, needleEnd
        )
    );
}