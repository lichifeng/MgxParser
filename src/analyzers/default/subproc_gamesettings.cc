/***************************************************************
 * \file       subproc_gamesettings.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"

/**
 * \brief      This method also builds some search patterns used to traverse in startinfo section
 * \todo       Game setting section is different in DE version, better look into it.
 */
void DefaultAnalyzer::AnalyzeGameSettings(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    // Preparation: Fetch search pattern trail from first player (normally GAIA)
    cursor_(initinfo_start_) >> (2 + num_players_ + 36 + 4 + 1);
    cursor_.ScanString();
    initinfo_searchpattern_trail_ = cursor_.Ptr();

    cursor_(gamesettings_start_) >> (64 + 4 + 8);
    // \todo not sure 12.20 is right, recanalyst uses 12.3
    if (IS_HD(version_code_) && save_version_ > 12.2001)
        cursor_ >> 16;
    if (!IS_AOK(version_code_))
        cursor_ >> map_id_;
    cursor_ >> difficulty_id_ >> lock_teams_;
    if (IS_DE(version_code_)) {
        cursor_ >> 29;
        if (save_version_ >= 13.0699)
            cursor_ >> 1;
        if (save_version_ >= 13.3399)
            cursor_ >> 132;
        if (save_version_ >= 20.0599)
            cursor_ >> 1;
        if (save_version_ >= 20.1599)
            cursor_ >> 4;
        if (save_version_ >= 25.0199)
            cursor_ >> 4 * 16;
        if (save_version_ >= 25.0599 && save_version_ < 26.2099)
            cursor_ >> 4;
    }

    uint16_t namelen;
    const uint8_t *name_ptr;
    for (int i = 0; i < 9; i++) {
        players[i].slot = i;
        name_ptr = cursor_.Ptr() + 8;
        // 先获得名字长度，再把 len + lenName + trailBytes 放到 pattern 里，
        // 供以后搜索使用， 然后再跳过或把名字转码保存下来。
        // trailBytes 只保存了一个指针，具体用后面的几个字节组装可以试下，看
        // 看性能表现，一般建议在[1, 6]个，第一个一般是0x16,第六个一般是
        // 0x21，中间一个4字节的数字则根据版本不同有所区别。
        // 注1：HD/DE版本中，startinfo里的名字数据和HD/DE header里的一
        // 样，和这里不一样。这里显示的玩家名是“玩家1”之类。
        // 注2：startinfo中字符串长度用2字节表示，这里用4字节。
        // \note 这里只有玩家才会在HD/DE专有区块有名字信息，GAIA和AI没有。
        // 注3：startinfo中的字符串有\0结尾，这里没有

        cursor_ >> players[i].index;
        if (players[i].name.length() > 0) {
            // Data from HD/DE header has higher priority
            cursor_ >> 4;
            namelen = players[i].name.length() + 1;
            players[i].searchpattern_.resize(2 + namelen + trailbyte_num_);
            memcpy(players[i].searchpattern_.data(), &namelen, 2);
            memcpy(players[i].searchpattern_.data() + 2, players[i].name.data(), namelen);
            memcpy(players[i].searchpattern_.data() + 2 + namelen, initinfo_searchpattern_trail_, trailbyte_num_);
            cursor_.ScanString();
        } else {
            cursor_ >> players[i].type_;
            // 两处字符串前面有\0，后面有的版本有，有的版本没有，需要判断处理
            namelen = *(uint32_t *) name_ptr;
            if ('\0' == *(name_ptr + 4 + namelen - 1)) {
                players[i].searchpattern_.resize(2 + namelen + trailbyte_num_);
                memcpy(players[i].searchpattern_.data() + 2, name_ptr + 4, namelen);
            } else {
                namelen += 1;
                players[i].searchpattern_.resize(2 + namelen + trailbyte_num_);
                players[i].searchpattern_[2 + namelen - 1] = '\0';
                memcpy(players[i].searchpattern_.data() + 2, name_ptr + 4, namelen - 1);
            }
            memcpy(players[i].searchpattern_.data(), &namelen, 2);
            memcpy(players[i].searchpattern_.data() + 2 + namelen, initinfo_searchpattern_trail_, trailbyte_num_);
            cursor_ >> players[i].name;
        }

        // sometimes ai included records have includeAI == true, mainly found in up record with barbarian AI
        if (players[i].type_ == 4)
            include_ai_ = 0x01;
    }
}