/***************************************************************
 * \file       subproc_scenario.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"
#include "searcher.h"

void DefaultAnalyzer::AnalyzeScenario(int debug_flag, bool brutal, float lower_limit, float upper_limit) {
    status_.debug_flag_ = debug_flag;

    // Try to locate scenario version data in DE (this float in DE varies
    // among minor game versions and relative stable in previous versions.)
    if (IS_DE(version_code_) || brutal) {
        //\warning AI files are very large.
        cursor_(victory_start_ - 4);
        while ((--cursor_)() != 0) {
            scenario_version_ = cursor_.Peek<float>();
            if (scenario_version_ > lower_limit && scenario_version_ < upper_limit) {
                scenario_start_ = cursor_() - 4; // 4 bytes are 00 00 00 00 before scenario version
                break;
            }
        }
    } else {
        // \todo 这里应该是13.3399还是13.3599?
        std::array<uint8_t, 4> scenario_separator;
        if (IS_AOK(version_code_)) {
            scenario_separator = {0x9a, 0x99, 0x99, 0x3f};
        } else {
            if (IS_HD(version_code_) && save_version_ > 11.9701) {
                if (save_version_ >= 12.3599) {
                    scenario_separator = {0xae, 0x47, 0xa1, 0x3f};
                } else {
                    scenario_separator = {0xa4, 0x70, 0x9d, 0x3f};
                }
            } else {
                scenario_separator = {0xf6, 0x28, 0x9c, 0x3f};
            }
        }

        auto haystack_begin = cursor_.RItr(victory_start_);
        auto haystack_end = cursor_.RItr(0);
        auto found = SearchPattern(
                haystack_begin,
                haystack_end,
                scenario_separator.rbegin(),
                scenario_separator.rend());

        if (found == haystack_end) {
            // \todo test/testRecords/Warning_aitest.mgx seems a record of early HD
            // versions with same scenarioVersion as AOC10C, need to look into it.
            AnalyzeScenario(100 + debug_flag, true, 1.19);
        } else {
            scenario_version_ = cursor_(haystack_end - found - 4).Peek<float>();
            scenario_start_ = cursor_() - 4;
        }
    }

    if (!scenario_start_)
        throw std::string("Failed to find scenario section.");

    // Read scenario header info
    cursor_(scenario_start_) >> 4433 >> scenario_filename_;

    if (IS_DE(version_code_)) {
        cursor_ >> 64;
        if (save_version_ >= 13.3399)
            cursor_ >> 64;
    }

    message_start_ = cursor_();
}