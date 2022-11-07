/**
 * \file       subProcLobby.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeLobby(int debug_flag) {
    status_.debug_flag_ = debug_flag;
    cursor_(lobby_start_);

    if (save_version_ >= 13.3399)
        cursor_ >> 5;
    if (save_version_ >= 20.0599)
        cursor_ >> 9;
    if (save_version_ >= 26.1599)
        cursor_ >> 5;
    for (size_t i = 1; i < 9; i++) {
        if (players[i].resolved_teamid_ != 255) {
            cursor_ >> 1;
        } else {
            cursor_ >> players[i].resolved_teamid_;
        }
    }
    if (save_version_ < 12.2999)
        cursor_ >> 1;
    cursor_ >> revealmap_
            >> fogofwar_
            >> map_size_;
    if (population_limit_ == UINT32_INIT) {
        cursor_ >> population_limit_;
        if (population_limit_ < 40)
            population_limit_ *= 25;
    } else {
        cursor_ >> 4;
    }
    cursor_ >> game_type_ >> lock_diplomacy_;
    if (IS_HD(version_code_) || IS_DE(version_code_)) {
        cursor_ >> treatyLength >> 4; // cheat code used, what's this?
        if (save_version_ >= 13.1299)
            cursor_ >> 4;
        if (save_version_ >= 25.2199)
            cursor_ >> 1;
    }

    // Read lobby(pregame) talks
    if (!IS_AOK(version_code_)) {
        Chat tmp_chat;
        int32_t num_chat;

        cursor_ >> num_chat;
        if (num_chat > 1000) { // numChat is considered <= 50. Incase future versions set a bigger limit, use 1000 here.
            num_chat = 50;
        }
        while (num_chat-- > 0 && (cursor_() + 4 <= body_start_)) {
            cursor_ >> tmp_chat.msg;
            if (tmp_chat.msg.length() > 3) { // 有时候有长度为1，内容为空的。一般来说因为有“@#2”的存在，至少为3
                chat.emplace_back(tmp_chat);
            } else {
                if (!IS_DE(version_code_))
                    ++num_chat; /// \todo Verify this.
            }
        }
    }

    if (IS_DE(version_code_) && (cursor_() + 4 <= body_start_))
        cursor_ >> de_mapseed_;
}