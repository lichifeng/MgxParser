/**
 * \file       subProcessVictorySettings.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_victorySettingsAnalyzer() {
    _curPos = _victoryStartPos;
    _skip(4);
    _readBytes(4, &victoryIsConquest);
    _skip(4);
    _readBytes(4, &victoryRelics);
    _skip(4);
    _readBytes(4, &victoryExplored);
    _skip(4);
    _readBytes(4, &victoryAnyOrAll);
    _readBytes(4, &victoryMode);
    _readBytes(4, &victoryScore);
    _readBytes(4, &victoryTime);
    
    // Do a simple check with Unknown section
    if (*(uint32_t*)_curPos != 3) {
        message.append("[WARN] Check bytes following victory section seems wrong, expected 03 00 00 00.");
    }
}