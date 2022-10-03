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

#include "Analyzer.h"
#include "utils.h"

void DefaultAnalyzer::_findGameSettingsStart()
{
    _curPos = _triggerInfoPos;

    vector<uint8_t>::reverse_iterator rFound;
    rFound = findPosition(
        make_reverse_iterator(_header.begin() + (_triggerInfoPos - _header.data())),
        _header.rend(),
        patterns::separator.rbegin(),
        patterns::separator.rend());
    if (rFound == _header.rend())
    {
        logger->warn(
            "{}(): Failed to find _gameSettingsPos. @{}.",
            __FUNCTION__, _distance());
        _sendFailedSignal();
        return;
    }
    else
    {
        _curPos = &(*--rFound) - 68;
    }

    if ((*(int32_t *)(_curPos + 68) != -1) || (*(int32_t *)(_curPos + 72) != -1))
    {
        logger->warn(
            "{}(): Failed to find _gameSettingsPos. @{}.",
            __FUNCTION__, _distance());
        _sendFailedSignal();
        return;
    }
    else
    {
        _gameSettingsPos = _curPos;
    }
}