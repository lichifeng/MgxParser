/**
 * \file       version.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      Version related stuffs.
 * \version    0.1
 * \date       2022-09-23
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#pragma once

#define IS_TRIAL(v) (v==0&&v==1)
#define IS_AOK(v) v==5
#define IS_AOC(v) (v>=10&&v<20)
#define IS_AOFE(v) v==20
#define IS_UP(v) (v>=50&&v<100)
#define IS_MCP(v) v==100
#define IS_HD(v) (v>=110&&v<150)
#define IS_DE(v) v==200

enum {
    // trial: >=0 && <5
    AOKTRIAL        = 0,
    AOCTRIAL        = 1,
    // aok: >=5 && <10
    AOK             = 5,
    // aoc/aoc10/aoc10c: >=10 && <20
    AOC             = 10,
    AOC10           = 11,
    AOC10C          = 12,
    // aofe: >=20 && <50
    AOFE21          = 20,
    // up: >=50 && <100
    USERPATCH12     = 50,
    USERPATCH13     = 51,
    USERPATCH14RC1  = 52,
    USERPATCH14RC2  = 53,
    USERPATCH14     = 54,
    USERPATCH15     = 55,
    // mcp: >=100 && <110
    MCP             = 100,
    // hd: >=110 && <150
    HD              = 110,
    HD43            = 111,
    HD46_7          = 112, ///< 4.7, 4.6
    HD48            = 113, ///< 4.8
    HD50_6          = 114, ///< 5.0, 5.1, 5.1a, 5.3, 5.5, 5.6
    HD57            = 115, ///< 5.7
    HD58            = 116, ///< 5.8
    // de: >=200 && <250
    DE              = 200,
    // unsupported: =999
    UNSUPPORTED     = 999
} VER;