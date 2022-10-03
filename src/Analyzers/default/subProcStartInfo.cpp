/**
 * \file       subProcStartInfo.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "Analyzer.h"

void DefaultAnalyzer::_startInfoAnalyzer()
{
    if (_startInfoPos)
    {
        _curPos = _startInfoPos;
    }
    else
    {
        return;
    }

    uint32_t numHeaderData = *(uint32_t *)(_startInfoPatternTrail + 1);

    for (auto &p : players)
    {
        if (p.initialDataFound() && p.valid())
        {
            _curPos = _header.data() + p.dataOffset;
            _skip(2 + numPlayers + 36 + 4 + 1);
            _skipPascalString();
            _skip(762);

            // float x, y;
            // while(true) {
            //     x = *(float*)_curPos;
            //     y = *(float*)(_curPos+4);
            //     if (x < mapCoord[0] && x > 0 && y < mapCoord[1] && y > 0) {
            //         logger->info("Found initPos:[{}, {}] in DE!! @{}", x, y, _distance());
            //         break;
            //     }
            //     if (_remainBytes() > 0) ++_curPos;
            // }continue;

            if (saveVersion >= 11.7599)
                _skip(36);
            if (IS_DE(versionCode) || IS_HD(versionCode))
                _skip(4 * (numHeaderData - 198));
            if (versionCode == USERPATCH15 || versionCode == MCP)
            {
                _readBytes(4, &p.modVersionID);
                _skip(4 * 6 + 4 * 7 + 4 * 28);
            }
            if (versionCode == MCP)
                _skip(4 * 65);
            _skip(1);
            _readBytes(8, p.initCamera);
            logger->info("p1 {}: X:{} Y: {} @{}", p.name, p.initCamera[0], p.initCamera[1], _distance());
            
            // Do some check here
            if (
                p.initCamera[0] < 0 || p.initCamera[0] > mapCoord[0] || p.initCamera[1] < 0 || p.initCamera[1] > mapCoord[1])
            {
                logger->warn(
                    "{}(): Bad init camera was found. @{} #{}.",
                    __FUNCTION__, _distance(), _debugFlag);
                _sendFailedSignal();
                p.initCamera[0] = p.initCamera[1] = -1.0;
                continue;
            }

            if (!IS_AOK(versionCode))
                _skip(*(int32_t *)_curPos * 8 + 4);
            _skip(4 + 1);
            _readBytes(1, &p.civID);
            _skip(3);
            _readBytes(1, &p.colorID);
        }
    }
}