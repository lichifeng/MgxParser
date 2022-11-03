/**
 * \file       subProcFindDisables.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::_findDisablesStart(int debugFlag)
{
    _debugFlag = debugFlag;

    _curPos = _gameSettingsPos;

    if (!IS_DE(version_code_))
    {
        if (IS_HD(version_code_) && save_version_ < 12.3399 && save_version_ > 11.9701)
        {
            _disablesStartPos = _curPos - 5396;
        }
        else
        {
            _disablesStartPos = _curPos - 5392;
        }
    }
    else
    {
        _disablesStartPos = _curPos - 276;
    }

    if (IS_HD(version_code_) && save_version_ >= 12.3399)
    {
        _disablesStartPos -= 644;
    }

    // Check if at correct point
    if (*(int32_t *)_disablesStartPos != -99)
    {
        _curPos -= 4;                     /// 先后退四个字节再开始查找
        for (size_t i = 0; i < 8000; i++) /// \todo 这个数随便定的，.mgx2文件里好像差几个字节，不管了，反正特殊情况就查找吧
        {
            if (*(int32_t *)_curPos == -99)
            {
                _disablesStartPos = _curPos;
                break;
            }
            --_curPos;
        }

        if (*(int32_t *)_disablesStartPos != -99)
        {
            logger_->warn(
                "{}(): _disablesStartPos failed pattern check. @{}.",
                __FUNCTION__, _distance());
            _sendExceptionSignal();
            return;
        }
        else
        {
            logger_->warn(
                "{}(): _disablesStartPos was reached by reverse search, {} bytes from game game settings section. @{}.",
                __FUNCTION__, _gameSettingsPos - _disablesStartPos, _distance());
            return;
        }
    }
}