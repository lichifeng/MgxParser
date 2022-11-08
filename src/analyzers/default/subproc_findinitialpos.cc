/***************************************************************
 * \file       subproc_findinitialpos.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"
#include "searcher.h"

void DefaultAnalyzer::FindInitialDataPosition(int debug_flag) {
    status_.debug_flag_ = debug_flag;
    cursor_(initinfo_start_ + 2 + num_players_ + 36 + 4 + 1);

    easyskip_base_ += map_coord_[0] * map_coord_[1];
    auto haystack_begin = cursor_.Itr() + easyskip_base_;
    auto haystack_end = cursor_.Itr() +
                        (scenario_start_ ? scenario_start_
                                         : victory_start_ ? victory_start_
                                                          : disabledtechs_start_ ? disabledtechs_start_
                                                                                 : gamesettings_start_)
                        - num_players_ * 1817; // Achievement section is 1817 * numPlayers bytes

    // Length of every player's data is at least easySkip bytes (and ususally much
    // more), we can use this to escape unnecessary search
    // First player don't need a search (and cannot, 'cuz different behavior
    // among versions)
    players[0].data_offset_ = cursor_();

    auto found = haystack_begin;
    bool index_found[9] = {false};
    for (size_t i = 1; i < 9; i++) {
        if (!players[i].Valid() || players[i].index < 0 || players[i].index > 8 || index_found[players[i].index])
            continue;

        found = SearchPattern(
                haystack_begin, haystack_end,
                players[i].searchpattern_.cbegin(),
                players[i].searchpattern_.cend());
        if (found == haystack_end) {
            // \todo 如果只是当前这个没有找到，那是不是应该退回查找起始位置呢？
            // 多控的情况下只有一个玩家会有这里的信息，他们的顺序难道是固定的？
            found = SearchPattern(
                    haystack_begin, haystack_end,
                    players[i].searchpattern_.cend() - trailbyte_num_,
                    players[i].searchpattern_.cend());

            if (found != haystack_end) {
                cursor_.Itr(found - 3);
                // 倒着往前找字符串，一般字符串最后一个字节是'\0'，表示长度又有两个字节，所以往前退3个字节开始查找
                for (size_t j = 3; j < 300; ++j) {
                    if (2 + cursor_.Peek<uint16_t>() == j) {
                        players[i].data_offset_ = cursor_();
                        cursor_ >> players[i].name;
                        break;
                    }
                    --cursor_;
                }
            }
        } else {
            cursor_.Itr(found);
            players[i].data_offset_ = cursor_();
        }

        if (0 != players[i].data_offset_) {
            index_found[players[i].index] = true;
            haystack_begin = found + easyskip_base_;
        }
    }
}