/***************************************************************
 * \file       action_handlers.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <cstdint>
#include "../analyzer.h"
#include "auxiliary.h"

/**
 * https://github.com/stefan-kolb/aoc-mgx-format
 */
void DefaultAnalyzer::HandleAction(uint32_t &cmdlen) {
    uint8_t player_id_slot_[2];
    int8_t player_slot, cmd;
    int16_t tech_id, len;

    cursor_ >> cmd;
    player_slot = cursor_.Peek<int8_t>();
    len = cursor_.Peek<int16_t>(1);
    if (3 + len == cmdlen) {
        // \todo build 71094 action ?
    }


    switch (cmd) {
        case COMMAND_RESIGN:
            // int8 :player_number
            // int8 :player_id
            // int32 :disconnect
            cursor_ >> player_id_slot_;
            if (player_id_slot_[0] >= 0 && player_id_slot_[0] < 9 && players[player_id_slot_[0]].Valid()) {
                players[player_id_slot_[0]].resigned_ = duration_;
                if (!players[player_id_slot_[0]].disconnected_)
                    players[player_id_slot_[0]].disconnected_ = (bool) cursor_.Peek<int32_t>();
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

        default:
            break;
    }
}