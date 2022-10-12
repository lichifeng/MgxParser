/**
 * \file       subProcFindVictoryStart.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_findVictoryStart() {
    _victoryStartPos = _disablesStartPos - 12544 - 44;
    
    // Check if at correct point
    if (*(int32_t*)_victoryStartPos != -99) {
        throw(ParserException("[WARN] Check bytes not valid, _victoryStartPos seems not good. \n")); // 9d ff ff ff
    } else {
        message.append("[INFO] Reach _victoryStartPos and passed validation. \n");
    }
}