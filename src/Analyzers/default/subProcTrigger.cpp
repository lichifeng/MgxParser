/**
 * \file       subProcessTriggerInfo.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

/**
 * \brief      This method is not fully tested
 * 
 */
void DefaultAnalyzer::_triggerInfoAnalyzer() {
    _curPos = _triggerInfoPos;

    int conditionSize = 72;
    int32_t
        numTriggers,
        descriptionLen,
        nameLen,
        numEffects,
        numSelectedObjs,
        textLen,
        soundFilenameLen,
        numConditions;
    bool isHDPatch4 = saveVersion >= 12.3399;
    
    _skip(1);

    if (isHDPatch4) conditionSize += 8;

    _readBytes(4, &numTriggers);
    for (int i = 0; i < numTriggers; i++)
    {
        _skip(18);
        _readBytes(4, &descriptionLen);
        _skip(descriptionLen > 0 ? descriptionLen : 0);
        _readBytes(4, &nameLen);
        _skip(nameLen > 0 ? nameLen : 0);
        _readBytes(4, &numEffects);
        for (int i = 0; i < numEffects; i++)
        {
            _skip(24);
            _readBytes(4, &numSelectedObjs);
            numSelectedObjs = numSelectedObjs == -1 ? 0 : numSelectedObjs;
            _skip(isHDPatch4 ? 76 : 72);
            _readBytes(4, &textLen);
            _skip(textLen > 0 ? textLen : 0);
            _readBytes(4, &soundFilenameLen);
            _skip(soundFilenameLen > 0 ? soundFilenameLen : 0);
            _skip(numSelectedObjs * 4);
        }
        _skip(numEffects * 4);
        _readBytes(4, &numConditions);
        _skip(numConditions * conditionSize + numConditions * 4);
    }

    if (numTriggers > 0) _skip(numTriggers * 4);

    if (IS_DE(versionCode)) _skip(1032);

    _lobbyStartPos = _curPos;
}