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

namespace patterns
{
  vector<uint8_t> HDseparator = {0xa3, 0x5f, 0x02, 0x00};
  vector<uint8_t> HDStringSeparator = {0x60, 0x0a};
  vector<uint8_t> AIdataUnknown = {0x08, 0x00};
  vector<uint8_t> ZEROs_4096(4096, 0x00);
  vector<uint8_t> FFs_500(500, 0xff);
  vector<uint8_t> AIDirtyFix = {0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x10, 0x27};
  vector<uint8_t> gameSettingSign = {0x00, 0xe0, 0xab, 0x45};
  vector<uint8_t> gameSettingSign1 = {0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xf9, 0x3f};
  vector<uint8_t> separator = {0x9d, 0xff, 0xff, 0xff};
  vector<uint8_t> scenarioConstantAOC = {0xf6, 0x28, 0x9c, 0x3f};
  vector<uint8_t> scenarioConstantHD = {0xae, 0x47, 0xa1, 0x3f};
  vector<uint8_t> scenarioConstantAOK = {0x9a, 0x99, 0x99, 0x3f};
  vector<uint8_t> scenarioConstantMGX2 = {0xa4, 0x70, 0x9d, 0x3f};

  constexpr char zh_pattern[] = {
      static_cast<char>(0xb5),
      static_cast<char>(0xd8),
      static_cast<char>(0xcd),
      static_cast<char>(0xbc),
      static_cast<char>(0xc0),
      static_cast<char>(0xe0),
      static_cast<char>(0xb1),
      static_cast<char>(0xf0),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // '地图类别: '
  constexpr char zh_utf8_pattern[] = {
      static_cast<char>(0xe5),
      static_cast<char>(0x9c),
      static_cast<char>(0xb0),
      static_cast<char>(0xe5),
      static_cast<char>(0x9b),
      static_cast<char>(0xbe),
      static_cast<char>(0xe7),
      static_cast<char>(0xb1),
      static_cast<char>(0xbb),
      static_cast<char>(0xe5),
      static_cast<char>(0x9e),
      static_cast<char>(0x8b),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // '地图类型: '
  constexpr char zh_wide_pattern[] = {
      static_cast<char>(0xb5),
      static_cast<char>(0xd8),
      static_cast<char>(0xcd),
      static_cast<char>(0xbc),
      static_cast<char>(0xc0),
      static_cast<char>(0xe0),
      static_cast<char>(0xb1),
      static_cast<char>(0xf0),
      static_cast<char>(0xa3),
      static_cast<char>(0xba)}; // '地图类别：'
  constexpr char zh_tw_pattern[] = {
      static_cast<char>(0xa6),
      static_cast<char>(0x61),
      static_cast<char>(0xb9),
      static_cast<char>(0xcf),
      static_cast<char>(0xc3),
      static_cast<char>(0xfe),
      static_cast<char>(0xa7),
      static_cast<char>(0x4f),
      static_cast<char>(0xa1),
      static_cast<char>(0x47)}; // '地圖類別：'
  constexpr char br_pattern[] = "Tipo de Mapa: ";
  constexpr char de_pattern[] = "Kartentyp: ";
  constexpr char en_pattern[] = "Map Type: ";
  constexpr char es_pattern[] = "Tipo de mapa: ";
  constexpr char fr_pattern[] = "Type de carte : ";
  constexpr char it_pattern[] = "Tipo di mappa: ";
  constexpr char jp_pattern[] = {
      static_cast<char>(0x83),
      static_cast<char>(0x7d),
      static_cast<char>(0x83),
      static_cast<char>(0x62),
      static_cast<char>(0x83),
      static_cast<char>(0x76),
      static_cast<char>(0x82),
      static_cast<char>(0xcc),
      static_cast<char>(0x8e),
      static_cast<char>(0xed),
      static_cast<char>(0x97),
      static_cast<char>(0xde),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // 'マップの種類:'
  constexpr char jp_utf8_pattern[] = {
      static_cast<char>(0xe3),
      static_cast<char>(0x83),
      static_cast<char>(0x9e),
      static_cast<char>(0xe3),
      static_cast<char>(0x83),
      static_cast<char>(0x83),
      static_cast<char>(0xe3),
      static_cast<char>(0x83),
      static_cast<char>(0x97),
      static_cast<char>(0xe3),
      static_cast<char>(0x81),
      static_cast<char>(0xae),
      static_cast<char>(0xe7),
      static_cast<char>(0xa8),
      static_cast<char>(0xae),
      static_cast<char>(0xe9),
      static_cast<char>(0xa1),
      static_cast<char>(0x9e),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // 'マップの種類: '
  constexpr char ko_pattern[] = {
      static_cast<char>(0xc1),
      static_cast<char>(0xf6),
      static_cast<char>(0xb5),
      static_cast<char>(0xb5),
      static_cast<char>(0x20),
      static_cast<char>(0xc1),
      static_cast<char>(0xbe),
      static_cast<char>(0xb7),
      static_cast<char>(0xf9),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // '지도 종류: '
  constexpr char ko_utf8_pattern[] = {
      static_cast<char>(0xec),
      static_cast<char>(0xa7),
      static_cast<char>(0x80),
      static_cast<char>(0xeb),
      static_cast<char>(0x8f),
      static_cast<char>(0x84),
      static_cast<char>(0x20),
      static_cast<char>(0xec),
      static_cast<char>(0xa2),
      static_cast<char>(0x85),
      static_cast<char>(0xeb),
      static_cast<char>(0xa5),
      static_cast<char>(0x98),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // '지도 종류: '
  constexpr char nl_pattern[] = "Kaarttype: ";
  constexpr char ru_pattern[] = {
      static_cast<char>(0xd2),
      static_cast<char>(0xe8),
      static_cast<char>(0xef),
      static_cast<char>(0x20),
      static_cast<char>(0xea),
      static_cast<char>(0xe0),
      static_cast<char>(0xf0),
      static_cast<char>(0xf2),
      static_cast<char>(0xfb),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // 'Тип карты: '
  constexpr char ru_utf8_pattern[] = {
      static_cast<char>(0xd0),
      static_cast<char>(0xa2),
      static_cast<char>(0xd0),
      static_cast<char>(0xb8),
      static_cast<char>(0xd0),
      static_cast<char>(0xbf),
      static_cast<char>(0x20),
      static_cast<char>(0xd0),
      static_cast<char>(0xba),
      static_cast<char>(0xd0),
      static_cast<char>(0xb0),
      static_cast<char>(0xd1),
      static_cast<char>(0x80),
      static_cast<char>(0xd1),
      static_cast<char>(0x82),
      static_cast<char>(0xd1),
      static_cast<char>(0x8b),
      static_cast<char>(0x3a),
      static_cast<char>(0x20)}; // 'Тип карты: '
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
std::string hexStr(unsigned char *&data, int len, bool skip = false)
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

template <typename T>
T findPosition(T haystackBeg, T haystackEnd, T needleBeg, T needleEnd)
{
  return std::search(
      haystackBeg, haystackEnd,
      std::boyer_moore_searcher(
          needleBeg, needleEnd));
}