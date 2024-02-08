/***************************************************************
 * \file       action_handlers.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <cstdint>
#include <string>
#include <utility>
#include "../analyzer.h"
#include "auxiliary.h"

/**
 * https://github.com/stefan-kolb/aoc-mgx-format
 */
void DefaultAnalyzer::HandleAction(uint32_t &cmdlen) {
    uint8_t player_id_slot[2];
    int8_t player_slot;
    uint8_t cmd; // https://stackoverflow.com/questions/63452441/error-result-of-comparison-of-constant-255-with-expression-of-type-char-is-al
    int16_t tech_id, len;

    cursor_ >> cmd;
    // de build 71094 https://github.com/happyleavesaoc/aoc-mgz/commit/19b75b3b7e51e254075644ea2be96fdb0a7477b1
    // player_slot = cursor_.Peek<int8_t>();
    // len = cursor_.Peek<int16_t>(1);
    // if (3 + len == cmdlen) {
    //     // \todo build 71094 action ?
    // }

    switch (cmd) {
        case COMMAND_RESIGN:
            // int8 :player_number
            // int8 :player_id
            // int32 :disconnect
            cursor_ >> player_id_slot;
            if (player_id_slot[0] >= 0 && player_id_slot[0] < 9 && players[player_id_slot[0]].Valid()) {
                players[player_id_slot[0]].resigned_ = duration_;
                if (!players[player_id_slot[0]].disconnected_)
                    players[player_id_slot[0]].disconnected_ = (bool) cursor_.Peek<int32_t>();
            }
            break;

        case COMMAND_RESEARCH:
            // int24 :zero
            // int32 :building_id
            // int8 :player_number
            // int8 :zero2
            // int16 :technology_id
            // int32 :const
            cursor_ >> 7 >> player_slot;
            if (player_slot < 0 || player_slot > 8 || !players[player_slot].Valid())
                break;
            cursor_ >> 1 >> tech_id;

            if (RESEARCH_FEUDAL == (int) tech_id) {
                players[player_slot].feudal_time_ = duration_ + 0.5 + 130000;
            } else if (RESEARCH_CASTLE == (int) tech_id) {
                players[player_slot].castle_time_ =
                        duration_ + 0.5 + (8 == players[player_slot].civ_id_ ? 160000 : 160000 / 1.10);
            } else if (RESEARCH_IMPERIAL == (int) tech_id) {
                players[player_slot].imperial_time_ =
                        duration_ + 0.5 + (8 == players[player_slot].civ_id_ ? 190000 : 190000 / 1.10);
            }
            break;

        case COMMAND_MOVE:
            if (earlymove_count_ < EARLYMOVE_USED) {
                earlymove_cmd_[earlymove_count_] = cursor_.Ptr();
                earlymove_time_[earlymove_count_] = duration_;
                ++earlymove_count_;
            }
            break;

        case COMMAND_POSTGAME:
            // Another way to reach postgame data by searching:
            // https://github.com/happyleavesaoc/aoc-mgz/blob/2a007a4b8556230a71df5ee786f3dfdd0c7e60e5/mgz/util.py
            // https://github.com/happyleavesaoc/aoc-mgz/blob/master/mgz/body/actions.py
            // https://github.com/lichifeng/RecAnalyst2/blob/master/src/Analyzers/PostgameDataAnalyzer.php
            uint8_t civ_id, color_id, team_id;
            cursor_ >> 3 >> 32 >> 4
                    >> 4 // duration in seconds
                    >> 4 >> 8 >> 4 >> 4 >> 12;
            for (int i = 0; i < 8; i++) {
                std::string tmp_name;
                for (int j = 16; j > 0; j--) {
                    if (*(cursor_.Ptr() + j - 1) != 0x00) {
                        tmp_name.assign((const char *)cursor_.Ptr(), j);
                        break;
                    }
                }
                if (tmp_name.empty()) continue;
                cursor_.FixEncoding(tmp_name);
                cursor_ >> 16 >> 2 >> 8 * 2 >> 1
                        >> civ_id >> color_id >> team_id
                        >> 10
                        >> 8 * 2 >> 32 // military stats
                        >> 28 >> 8 * 2 // economy stats
                        >> 19 >> 1 // tech stats
                        >> 8 // social stats
                        >> 84; // padding
                for (auto &p : players) {
                    if (!p.updatedByPostgameBlock && p.name == tmp_name) {
                        p.civ_id_ = civ_id;
                        p.color_id_ = color_id > 0 ? color_id - 1 : 10; // Don't change this casually! This will infect guid!
                        p.resolved_teamid_ = team_id;
                        p.updatedByPostgameBlock = true;
                        break;
                    }
                }
            }
            cursor_ >> 4;
            break;

        default:
            break;
    }
}