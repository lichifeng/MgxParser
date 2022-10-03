/**
 * \file       subProcFindVictory.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "Analyzer.h"

void DefaultAnalyzer::_findVictoryStart()
{
    _victoryStartPos = _disablesStartPos - 12544 - 44;

    // Check if at correct point
    if (*(int32_t *)_victoryStartPos != -99)
    {
        logger->warn(
            "{}(): _victoryStartPos failed pattern check. @{}.", // 9d ff ff ff
            __FUNCTION__, _distance());
        _sendFailedSignal();
        return;
    }
}