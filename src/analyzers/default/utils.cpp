/**
 * \file       utils.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-03
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include <string>
#include <vector>

#include "utils.h"

namespace patterns
{
  std::vector<uint8_t> HDseparator = {0xa3, 0x5f, 0x02, 0x00};
  std::vector<uint8_t> HDStringSeparator = {0x60, 0x0a};
  std::vector<uint8_t> ZEROs_4096(4096, 0x00);
  std::vector<uint8_t> FFs_500(500, 0xff);
  std::vector<uint8_t> gameSettingSign = {0x00, 0xe0, 0xab, 0x45};
  std::vector<uint8_t> gameSettingSign1 = {0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xf9, 0x3f};
  std::vector<uint8_t> separator = {0x9d, 0xff, 0xff, 0xff};
  std::vector<uint8_t> scenarioConstantAOC = {0xf6, 0x28, 0x9c, 0x3f};
  std::vector<uint8_t> scenarioConstantHD = {0xae, 0x47, 0xa1, 0x3f};
  std::vector<uint8_t> scenarioConstantAOK = {0x9a, 0x99, 0x99, 0x3f};
  std::vector<uint8_t> scenarioConstantMGX2 = {0xa4, 0x70, 0x9d, 0x3f};
}

/**
 * \brief      Use to convert bytes to hex string, mainly used to generate guid
 * in HD versions.
 *
 * \param      data                Normally a reference to _curPos(pointer to
 * current reading position). !! Must be a reference, otherwise skip will not work.
 * \param      len                 Bytes to convert
 * \param      skip                Skip read bytes (len bytes)
 * \return     std::string         Generated string representitive of bytes
 */
std::string BytesToHex(const unsigned char *&data, int len, bool skip)
{
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i)
  {
    s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  if (skip)
    data += len;
  return s;
}