/**
 * \file       subProcessScenarioHeader.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_scenarioHeaderAnalyzer() {
    uint16_t filenameLen;
    _curPos = _scenarioHeaderPos;
    _skip(4433);
    _readBytes(2, &filenameLen);
    if (filenameLen > 224) throw(AnalyzerException("[WARN] scenarioFilename is unsually long. \n"));
    scenarioFilename.assign((char*)(_curPos+2), filenameLen);
    _skip(filenameLen);
    if (IS_DE(versionCode)) {
        _skip(64);
        if (saveVersion >= 13.3399) {
            _skip(64);
        }
    }
    _messagesStartPos = _curPos;
}