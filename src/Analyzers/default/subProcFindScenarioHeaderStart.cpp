/**
 * \file       subProcFindScenarioHeaderStart.cpp
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

void DefaultAnalyzer::_findScenarioHeaderStart() {
    // Try to locate scenario version data in DE (this float in DE varies
    // among minor game versions and relative stable in previous versions.)
    if (IS_DE(versionCode)) {
        size_t searchSpan = 8000; // usually 5500~6500
        auto startPoint = _victoryStartPos - searchSpan;
        _curPos = startPoint;
        for (size_t i = 0; i < searchSpan; i++)
        {
            scenarioVersion = *(float*)startPoint;
            if (scenarioVersion > 1.35 && scenarioVersion < 1.55) {
                _scenarioHeaderPos = _curPos = startPoint - 4; // 4 bytes are 00 00 00 00 before scenario version
                message.append("[INFO] Reach _scenarioHeaderPos by scenario version range test. \n");
                return;
            }
            ++startPoint;
        }
        throw(AnalyzerException("[WARN] Cannot find satisfied _scenarioHeaderPos in this DE version. \n"));
    }
    

    auto scenarioSeprator = IS_AOK(versionCode) ? \
        patterns::scenarioConstantAOK : \
        (
            IS_HD(versionCode) ? 
                (
                    /// \todo 这里应该是13.3399还是13.3599?
                    saveVersion >= 12.3599 ? \
                    patterns::scenarioConstantHD : \
                    patterns::scenarioConstantMGX2
                ) :
            patterns::scenarioConstantAOC
        );
        
    vector<uint8_t>::reverse_iterator rFound;
    rFound = findPosition(
        make_reverse_iterator(_header.begin() + (_victoryStartPos - _header.data())),
        _header.rend(),
        scenarioSeprator.rbegin(),
        scenarioSeprator.rend()
    );
    if (rFound == _header.rend()) {
        /// \todo aoc-mgz says aok scenario version can appear in UP
        throw(AnalyzerException("[WARN] Cannot find satisfied _scenarioHeaderPos. \n"));
    } else {
        _scenarioHeaderPos = _curPos = &*(--rFound) - 4 - scenarioSeprator.size();
        message.append("[INFO] Reach _scenarioHeaderPos and passed validation. \n");
    }
}