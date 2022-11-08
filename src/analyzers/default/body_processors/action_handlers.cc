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
void DefaultAnalyzer::HandleAction() {
    uint8_t resign_data[3];
    int8_t player_slot;
    int16_t tech_id;

    auto cmd = cursor_.Peek<uint8_t>();

    switch (cmd) {
        case COMMAND_RESIGN:
            // int8 :action_identifier
            // int8 :player_number
            // int8 :player_id
            // int32 :disconnect
            cursor_ >> resign_data;
            if (resign_data[1] >= 0 && resign_data[1] < 9 && players[resign_data[1]].Valid()) {
                players[resign_data[1]].resigned_ = duration_;
                if (!players[resign_data[1]].disconnected_)
                    players[resign_data[1]].disconnected_ = (bool) cursor_.Peek<int32_t>();
            }
            break;

        case COMMAND_RESEARCH:
            // int8 :action_identifier
            // int24 :zero
            // int32 :building_id
            // int8 :player_number
            // int8 :zero2
            // int16 :technology_id
            // int32 :const
            cursor_ >> 8 >> player_slot;
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