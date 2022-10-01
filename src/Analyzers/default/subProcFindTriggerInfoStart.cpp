/**
 * \file       subProcFindTriggerInfoStart.cpp
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

void DefaultAnalyzer::_findTriggerInfoStart() {

    // aok ~ hd: 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, f9, 3f (double 1.6)
    // de < 13.34: 00 e0 ab 45 + double 2.2
    // de >= 13.34:  \note Maybe the cutoff point is not 13.34
    //     13.34: 00 e0 ab 45 + padding(1) + double 2.4
    //     20.06: 00 e0 ab 45 + padding(1) + double 2.4
    //     20.16: 00 e0 ab 45 + padding(11) + double 2.4
    //     25.01: 00 e0 ab 45 + padding(11) + double 2.4
    //     25.02: 00 e0 ab 45 + padding(11) + double 2.4
    //     25.06: 00 e0 ab 45 + padding(11) + double 2.5
    //     25.22: 00 e0 ab 45 + padding(11) + double 2.6
    //     26.16: 00 e0 ab 45 + padding(11) + double 3.0
    //     26.18: 00 e0 ab 45 + padding(11) + double 3.0
    //     26.18: 00 e0 ab 45 + padding(11) + double 3.2

    vector<uint8_t>::reverse_iterator rFound;

    _curPos = _startInfoPos;

    if (IS_AOK(versionCode)) {
        rFound = findPosition(
            _header.rbegin(),
            _header.rend(), 
            patterns::gameSettingSign1.rbegin(),
            patterns::gameSettingSign1.rend()
        );
        if (rFound == _header.rend()) {
            throw(AnalyzerException("[WARN] Failed to find _triggerInfoPos (No sign found). \n"));
        }
        _triggerInfoPos = _curPos = &(*--rFound);
    } else {
        rFound = findPosition(
            _header.rbegin(),
            _header.rend(), 
            patterns::gameSettingSign.rbegin(),
            patterns::gameSettingSign.rend()
        );
        if (rFound == _header.rend()) {
            throw(AnalyzerException("[WARN] Failed to find _triggerInfoPos (No sign found). \n"));
        }
        _curPos = &(*--rFound);
        double signNum = 0.0;
        for (size_t i = 0; i <= triggerStartSearchRange; i++)
        {
            signNum = *(double*)_curPos;
            if (signNum >= 1.5 && signNum <= 10) {
                _triggerInfoPos = _curPos += 8;
                return;
            } else {
                ++_curPos;
            }
        }
        throw(AnalyzerException("[WARN] Failed to find _triggerInfoPos. \n"));
    }

    /// \todo Maybe I will deploy a brutal search for double float in [1.6, 10]
    /// if cannot find it easily?
}