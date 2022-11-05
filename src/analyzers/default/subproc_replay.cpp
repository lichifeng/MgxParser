/**
 * \file       subProcReplay.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeReplay(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(replay_start_)
            >> 12
            >> gameSpeed
            >> 29
            >> recPlayer
            >> numPlayers; // \note gaia included
    if (!IS_AOK(version_code_)) // \todo AOK condition not tested
        cursor_ >> instantBuild >> cheatsEnabled;
    cursor_ >> (2 + 58); // 2: gamemode?
    if (IS_DE(version_code_))
        cursor_ >> 8;

    map_start_ = cursor_();
}