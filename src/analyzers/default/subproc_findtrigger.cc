/**
 * \file       subProcFindTriggerInfo.cpp
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
#include "array"

void DefaultAnalyzer::FindTrigger(int debug_flag) {
    status_.debug_flag_ = debug_flag;

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

    auto haystack_begin = cursor_.RItr(body_start_);
    auto haystack_end = cursor_.RItr(0);
    if (IS_AOK(version_code_)) {
        std::array<uint8_t, 8> pattern = {0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xf9, 0x3f};

        auto found = SearchPattern(
                haystack_begin, haystack_end,
                pattern.rbegin(), pattern.rend());
        if (found == haystack_end)
            throw std::string("Failed to find trigger info section.");

        trigger_start_ = cursor_(haystack_end - found)();
        return;
    } else {
        std::array<uint8_t, 4> pattern = {0x00, 0xe0, 0xab, 0x45};

        auto found = SearchPattern(
                haystack_begin, haystack_end,
                pattern.rbegin(), pattern.rend());
        if (found == haystack_end)
            throw std::string("Failed to find trigger info section.");

        cursor_(haystack_end - found);

        double sign_num = 0.0;
        for (size_t i = 0; i <= triggerstart_search_range; i++) {
            sign_num = *(double *) cursor_.Ptr();
            if (sign_num >= 1.5 && sign_num <= 10) {
                trigger_start_ = cursor_() + 8;
                return;
            } else {
                cursor_ >> 1;
            }
        }

        throw std::string("Failed to find trigger info section.");
    }
}