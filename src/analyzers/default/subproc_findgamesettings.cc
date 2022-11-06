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

#include <array>
#include "analyzer.h"
#include "searcher.h"

void DefaultAnalyzer::FindGameSettings(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(trigger_start_);

    std::array<uint8_t, 4> pattern = {0x9d, 0xff, 0xff, 0xff};
    auto haystack_begin = cursor_.RItr(trigger_start_);
    auto haystack_end = cursor_.RItr(0);

    auto found = SearchPattern(
            haystack_begin, haystack_end,
            pattern.rbegin(), pattern.rend());
    if (found == haystack_end) {
        throw std::string("Failed to find game settings section.");
    } else {
        cursor_(haystack_end - found - 4 - 64);
    }

    gamesettings_start_ = cursor_();
}