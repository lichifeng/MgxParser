/**
 * \file       subProcFindStartInfoStart.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-09-30
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::_findStartInfoStart(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _readBytes(4, &restoreTime);

    uint32_t numParticles;
    _readBytes(4, &numParticles);
    _skip(numParticles * 27);

    // A checkpoint, expecting 10060 with AOK and 40600 with higher version
    if (*(uint32_t *)_curPos == 40600)
    {
        // ...
    }
    else if ((*(uint32_t *)_curPos == 10060) && IS_AOK(versionCode))
    {
        // ...
    }
    else
    {
        logger_->warn(
            "{}(): Cannot find expected check value 10060/40600 in start info section. @{}.",
            __FUNCTION__, _distance());
        _sendExceptionSignal();
        return;
    }

    // Pin start info position
    _startInfoPos = _curPos += 4;

    // Fetch search pattern trail from first player (normally GAIA), laterly
    // used in gamesettings section.
    _curPos += 2 + numPlayers + 36 + 4 + 1;
    _skipPascalString();

    _startInfoPatternTrail = _curPos;
}