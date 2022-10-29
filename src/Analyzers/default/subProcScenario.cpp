/**
 * \file       subProcScenario.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-03
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_scenarioHeaderAnalyzer(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _curPos = _scenarioHeaderPos;

    uint16_t filenameLen;
    _skip(4433);
    _readBytes(2, &filenameLen);

    if (filenameLen > 224)
    {
        logger->warn(
            "{}(): scenarioFilename is unsually long. @{}.",
            __FUNCTION__, _distance());
        _sendExceptionSignal();
        return;
    }

    scenarioFilename.assign((char *)(_curPos + 2), filenameLen);
    _skip(filenameLen);
    if (IS_DE(versionCode))
    {
        _skip(64);
        if (saveVersion >= 13.3399)
        {
            _skip(64);
        }
    }
    
    _messagesStartPos = _curPos;
}