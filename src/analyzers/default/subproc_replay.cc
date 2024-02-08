/***************************************************************
 * \file       subproc_replay.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeReplay(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(replay_start_)
            >> 12
            >> game_speed_
            >> 29
            >> rec_player_
            >> num_players_; // \note gaia included
    if (!IS_AOK(version_code_)) // \todo AOK condition not tested
        cursor_ >> instant_build_ >> cheats_enabled_;
    cursor_ >> (2 + 58); // 2: gamemode?
    if (IS_DE(version_code_))
        cursor_ >> 8;

    map_start_ = cursor_();
}