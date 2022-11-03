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

#include "analyzer.h"
#include "tools/searcher.h"

void DefaultAnalyzer::_findScenarioHeaderStart(int debugFlag, bool brutal, float lowerLimit, float upperLimit)
{
    _debugFlag = debugFlag;
    
    // Try to locate scenario version data in DE (this float in DE varies
    // among minor game versions and relative stable in previous versions.)
    if (IS_DE(version_code_) || brutal)
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
            if (scenarioVersion > lowerLimit && scenarioVersion < upperLimit)
            {
                _scenarioHeaderPos = _curPos - 4; // 4 bytes are 00 00 00 00 before scenario version
                return;
            }
        }
        logger_->warn(
                "{}(): Cannot find _scenarioHeaderPos in this {} version. @{}.",
                __FUNCTION__, version_code_, _distance());
        _sendExceptionSignal();
        return;
    }

    /// \todo 这里应该是13.3399还是13.3599?
    auto scenarioSeprator = IS_AOK(version_code_) ? \
        patterns::scenarioConstantAOK : \
        ((IS_HD(version_code_) && save_version_ > 11.9701) ? \
        (save_version_ >= 12.3599 ? \
        patterns::scenarioConstantHD : \
        patterns::scenarioConstantMGX2) : \
        patterns::scenarioConstantAOC);

    vector<uint8_t>::reverse_iterator rFound;
    rFound = SearchPattern(
            make_reverse_iterator(header_.begin() + (_victoryStartPos - header_.data())),
            header_.rend(),
            scenarioSeprator.rbegin(),
            scenarioSeprator.rend());

    if (rFound == header_.rend())
    {
        // \todo test/testRecords/Warning_aitest.mgx seems a record of early HD
        // versions with same scenarioVersion as AOC10C, need to look into it. 
        _findScenarioHeaderStart(100 + debugFlag, true, 1.19);
    }
    else
    {
        _scenarioHeaderPos = _curPos = &*(--rFound) - 4 - scenarioSeprator.size();
        scenarioVersion = scenarioVersion = *(float *)(_scenarioHeaderPos + 4);
    }
}