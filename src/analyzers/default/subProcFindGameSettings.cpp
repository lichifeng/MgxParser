/**
 * \file       subProcFindGameSettingsStart.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-09-30
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"
#include "tools/searcher.h"

void DefaultAnalyzer::_findGameSettingsStart(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _curPos = _triggerInfoPos;

    vector<uint8_t>::reverse_iterator rFound;
    rFound = SearchPattern(
            make_reverse_iterator(header_.begin() + (_triggerInfoPos - header_.data())),
            header_.rend(),
            patterns::separator.rbegin(),
            patterns::separator.rend());
    if (rFound == header_.rend())
    {
        logger_->warn(
            "{}(): Failed to find _gameSettingsPos by regular separator. @{}, Flag:{}.",
            __FUNCTION__, _distance(), _debugFlag);
        _sendExceptionSignal();
        return;
    }
    else
    {
        _curPos = &(*--rFound) - 68;
    }

    if ((*(int32_t *)(_curPos + 68) != -1) || (*(int32_t *)(_curPos + 72) != -1))
    {
        logger_->warn(
            "{}(): Find weired _gameSettingsPos. @{}, Flag:{}.",
            __FUNCTION__, _distance(), _debugFlag);
        //_sendFailedSignal();
    }
    
    _gameSettingsPos = _curPos;
}