/**
 * \file       subProcFindDisablesStart.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-30
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"

void DefaultAnalyzer::_findDisablesStart() {
    _curPos = _gameSettingsPos;
    if (!IS_DE(versionCode)) {
        if (IS_HD(versionCode) && saveVersion < 12.3399) {
            _disablesStartPos = _curPos - 5396;
        } else {
            _disablesStartPos = _curPos - 5392;
        }
    } else {
        _disablesStartPos = _curPos - 276;
    }
    if (IS_HD(versionCode) && saveVersion >= 12.3399) {
        _disablesStartPos -= 644;
    }
    
    // Check if at correct point
    if (*(int32_t*)_disablesStartPos != -99) {
        _curPos -= 4; /// 先后退四个字节再开始查找
        for (size_t i = 0; i < 8000; i++) /// \todo 这个数随便定的，.mgx2文件里好像差几个字节，不管了，反正特殊情况就查找吧
        {
            if (*(int32_t*)_curPos == -99)
            {
                _disablesStartPos = _curPos;
                break;
            }
            --_curPos;
        }
        if (*(int32_t*)_disablesStartPos != -99)
        {
            throw(AnalyzerException("[WARN] Check bytes not valid, _disablesStartPos seems not good. \n")); // 9d ff ff ff
        } else {
            message.append("[INFO] Reach _disablesStartPos by reverse search. \n");
        }
    } else {
        message.append("[INFO] Reach _disablesStartPos and passed validation. \n");
    }
}