/***************************************************************
 * \file       auxiliary.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_AUXILIARY_H_
#define MGXPARSER_AUXILIARY_H_

#define OP_COMMAND              0x01
#define OP_SYNC                 0x02
#define OP_VIEWLOCK             0x03
#define OP_CHAT                 0x04

#define COMMAND_RESIGN          0x0b
#define COMMAND_RESEARCH        0x65
#define COMMAND_TRAIN           0x77
#define COMMAND_TRAIN_SINGLE    0x64
#define COMMAND_BUILD           0x66
#define COMMAND_TRIBUTE         0x6c
#define COMMAND_POSTGAME        0xff
#define COMMAND_MOVE            0x03
#define COMMAND_SAVE            0x1b

#define RESEARCH_FEUDAL         101
#define RESEARCH_CASTLE         102
#define RESEARCH_IMPERIAL       103

// Version tools
#define IS_TRIAL(v) (v == 0 && v == 1)
#define IS_AOK(v) (v == 5)
#define IS_AOC(v) (v >= 10 && v < 20)
#define IS_AOFE(v) (v == 20)
#define IS_UP(v) (v >= 50 && v < 100)
#define IS_MCP(v) (v == 100)
#define IS_HD(v) (v >= 110 && v < 150)
#define IS_DE(v) (v == 200)

enum VERSIONCODE {
    // trial: >=0 && <5
    AOKTRIAL = 0,
    AOCTRIAL = 1,
    // aok: >=5 && <10
    AOK = 5,
    // aoc/aoc10/aoc10c: >=10 && <20
    AOC = 10,
    AOC10 = 11,
    AOC10C = 12,
    // aofe: >=20 && <50
    AOFE21 = 20,
    // up: >=50 && <100
    USERPATCH12 = 50,
    USERPATCH13 = 51,
    USERPATCH14RC1 = 52,
    USERPATCH14RC2 = 53,
    USERPATCH14 = 54,
    USERPATCH15 = 55,
    // mcp: >=100 && <110
    MCP = 100,
    // hd: >=110 && <150
    HD = 110,
    HD43 = 111,
    HD46_7 = 112, ///< 4.7, 4.6
    HD48 = 113,   ///< 4.8
    HD50_6 = 114, ///< 5.0, 5.1, 5.1a, 5.3, 5.5, 5.6, 5.7, 5.8
    HD57 = 115,   ///< 5.7
    HD58 = 116,   ///< 5.8
    // de: >=200 && <250
    DE = 200,
    // unsupported: = 999
    UNSUPPORTED = 999,
    UNDEFINED = 998
};

namespace patterns {
    constexpr uint8_t kZh[] = {0xb5, 0xd8, 0xcd, 0xbc, 0xc0, 0xe0, 0xb1, 0xf0, 0x3a, 0x20}; // '地图类别: '
    constexpr uint8_t kZhUtf8[] = {0xe5, 0x9c, 0xb0, 0xe5, 0x9b, 0xbe, 0xe7, 0xb1, 0xbb, 0xe5, 0x9e, 0x8b, 0x3a,
                                   0x20}; // '地图类型: '
    constexpr uint8_t kZhWide[] = {0xb5, 0xd8, 0xcd, 0xbc, 0xc0, 0xe0, 0xb1, 0xf0, 0xa3, 0xba}; // '地图类别：'
    constexpr uint8_t kZhTw[] = {0xa6, 0x61, 0xb9, 0xcf, 0xc3, 0xfe, 0xa7, 0x4f, 0xa1, 0x47}; // '地圖類別：'
    constexpr uint8_t kBr[] = "Tipo de Mapa: ";
    constexpr uint8_t kDe[] = "Kartentyp: ";
    constexpr uint8_t kEn[] = "Map Type: ";
    constexpr uint8_t kEs[] = "Tipo de mapa: ";
    constexpr uint8_t kFr[] = "Type de carte : ";
    constexpr uint8_t kIt[] = "Tipo di mappa: ";
    constexpr uint8_t kJp[] = {0x83, 0x7d, 0x83, 0x62, 0x83, 0x76, 0x82, 0xcc, 0x8e, 0xed, 0x97, 0xde, 0x3a,
                               0x20}; // 'マップの種類:'
    constexpr uint8_t kJpUtf8[] = {0xe3, 0x83, 0x9e, 0xe3, 0x83, 0x83, 0xe3, 0x83, 0x97, 0xe3, 0x81, 0xae, 0xe7,
                                   0xa8, 0xae, 0xe9, 0xa1, 0x9e, 0x3a, 0x20}; // 'マップの種類: '
    constexpr uint8_t kKo[] = {0xc1, 0xf6, 0xb5, 0xb5, 0x20, 0xc1, 0xbe, 0xb7, 0xf9, 0x3a, 0x20}; // '지도 종류: '
    constexpr uint8_t kKoUtf8[] = {0xec, 0xa7, 0x80, 0xeb, 0x8f, 0x84, 0x20, 0xec, 0xa2, 0x85, 0xeb, 0xa5, 0x98,
                                   0x3a, 0x20}; // '지도 종류: '
    constexpr uint8_t kNl[] = "Kaarttype: ";
    constexpr uint8_t kRu[] = {0xd2, 0xe8, 0xef, 0x20, 0xea, 0xe0, 0xf0, 0xf2, 0xfb, 0x3a,
                               0x20}; // 'Тип карты: '
    constexpr uint8_t kRuUtf8[] = {0xd0, 0xa2, 0xd0, 0xb8, 0xd0, 0xbf, 0x20, 0xd0, 0xba, 0xd0, 0xb0, 0xd1, 0x80,
                                   0xd1, 0x82, 0xd1, 0x8b, 0x3a, 0x20}; // 'Тип карты: '
}

#endif //MGXPARSER_AUXILIARY_H_