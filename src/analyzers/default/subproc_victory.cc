/***************************************************************
 * \file       subproc_victory.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeVictory(int debug_flag) {
    status_.debug_flag_ = debug_flag;
    cursor_(victory_start_ = disabledtechs_start_ - 12544 - 44)
            >> 4
            >> victory_isconquest_
            >> 4
            >> victory_relics_
            >> 4
            >> victory_explored_
            >> 4
            >> victory_any_or_all_
            >> victory_mode_
            >> victory_score_
            >> victory_time_;
}