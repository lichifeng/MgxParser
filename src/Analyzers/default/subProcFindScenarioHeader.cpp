/**
 * \file       subProcFindScenarioHeader.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "Analyzer.h"
#include "utils.h"

void DefaultAnalyzer::_findScenarioHeaderStart(int debugFlag)
{
    _debugFlag = debugFlag;
    
    // Try to locate scenario version data in DE (this float in DE varies
    // among minor game versions and relative stable in previous versions.)
    if (IS_DE(versionCode))
    {
        //auto startPoint = _victoryStartPos - scenarioSearchSpan;
        //_curPos = startPoint;
        //\warning AI file is very very large. This scenarioSearchSpan is a
        //stupid idea here. Just search to the top.
        //for (size_t i = 0; i < scenarioSearchSpan; i++)
        _curPos = _victoryStartPos - 4;
        while (--_curPos != _curStream->data())
        {
            scenarioVersion = *(float *)_curPos;
            if (scenarioVersion > 1.35 && scenarioVersion < 1.55)
            {
                _scenarioHeaderPos = _curPos - 4; // 4 bytes are 00 00 00 00 before scenario version
                return;
            }
        }
        logger->warn(
            "{}(): Cannot find _scenarioHeaderPos in this DE version. @{}.",
            __FUNCTION__, _distance());
        _sendFailedSignal();
        return;
    }

    /// \todo 这里应该是13.3399还是13.3599?
    auto scenarioSeprator = IS_AOK(versionCode) ? \
        patterns::scenarioConstantAOK : \
        (IS_HD(versionCode) ? \
        (saveVersion >= 12.3599 ? \
        patterns::scenarioConstantHD : \
        patterns::scenarioConstantMGX2) : \
        patterns::scenarioConstantAOC);

    vector<uint8_t>::reverse_iterator rFound;
    rFound = findPosition(
        make_reverse_iterator(_header.begin() + (_victoryStartPos - _header.data())),
        _header.rend(),
        scenarioSeprator.rbegin(),
        scenarioSeprator.rend());

    if (rFound == _header.rend())
    {
        /// \todo aoc-mgz says aok scenario version can appear in UP
        logger->warn(
            "{}(): Cannot find _scenarioHeaderPos(not DE). @{}.",
            __FUNCTION__, _distance());
        _sendFailedSignal();
        return;
    }
    else
    {
        _scenarioHeaderPos = _curPos = &*(--rFound) - 4 - scenarioSeprator.size();
    }
}