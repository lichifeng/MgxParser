/**
 * \file       subProcReplay.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "Analyzer.h"

void DefaultAnalyzer::_replayAnalyzer(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _skip(12);
    _readBytes(4, &gameSpeed);
    _skip(29);
    _readBytes(2, &recPlayer);
    _readBytes(1, &numPlayers); /// \note gaia included
    if (!IS_AOK(versionCode))
    { /// \todo AOK condition not tested
        _readBytes(1, &instantBuild);
        _readBytes(1, &cheatsEnabled);
    }
    _skip(2); //_readBytes(2, &gameMode);
    _skip(58);
    if (IS_DE(versionCode))
        _skip(8);
}