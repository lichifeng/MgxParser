/**
 * \file       subProcessStartInfo.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_startInfoAnalyzer() {
    uint32_t numHeaderData = *(uint32_t*)(_startInfoPatternTrail + 1);

    for (auto p : players) {
        if (p.initialDataFound() && p.valid()) {
            _curPos = _header.data() + p.dataOffset;
            _skip(2 + numPlayers + 36 + 4 + 1);
            _skipPascalString();
            _skip(762);
            if (saveVersion >= 11.7599) _skip(36);
            if (IS_DE(versionCode) || IS_HD(versionCode)) _skip(4 * (numHeaderData - 198));
            if (versionCode == USERPATCH15 || versionCode == MCP) {
                _readBytes(4, &p.modVersionID);
                _skip(4 * 7 + 4 * 28);
            }
            if (versionCode == MCP) _skip(4 * 65);
            _skip(1);
            _readBytes(8, p.initCamera);

            // Do some check here
            if (
                   p.initCamera[0] < 0 
                || p.initCamera[0] > mapCoord[0] 
                || p.initCamera[1] < 0
                || p.initCamera[1] > mapCoord[1]
            ) {
                message.append("[WARN] Bad init camera was found. \n");
                p.initCamera[0] = p.initCamera[1] = -1.0;
                continue;
            }


            if (!IS_AOK(versionCode)) _skip(*(float*)_curPos * 4);
            _skip(4 + 1);
            _readBytes(1, &p.civID);
            _skip(3);
            _readBytes(1, &p.colorID);
        }
    }
}