/***************************************************************
 * \file       subproc_ai.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <algorithm>
#include "analyzer.h"

void DefaultAnalyzer::AnalyzeAi(int debugFlag) {
    status_.debug_flag_ = debugFlag;

    cursor_(ai_start_) >> include_ai_;
    if (!include_ai_) {
        replay_start_ = cursor_();
        return;
    }

    uint16_t num_ai_strings;
    cursor_ >> 2
            >> num_ai_strings
            >> 4;
    for (uint32_t i = 0; i < num_ai_strings; i++) {
        cursor_.ScanString();
    }

    if (IS_DE(version_code_)) {
        cursor_ >> 5;
    } else {
        cursor_ >> 4; // \todo 验证一下各版本的情况
    }
    cursor_ >> 2; // 08 00

    // AI Data
    if (IS_DE(version_code_)) { // \todo this takes too long, not acceptable
        auto found = search_n(cursor_.Itr(), cursor_.Itr(body_start_), 4096, 0x00);
        cursor_(found) >> 4096;

//        auto curItr = header_.begin() + _distance();
//        /// \todo 这里用了非常不确定的方法来跳过，没有充分验证。没有直接查找4096个0X00是出于性能考虑。
//        patterns::FFs_500.resize(1000, 0x00);
//        curItr = SearchPattern(
//                curItr, header_.end(),
//                patterns::FFs_500.begin(),
//                patterns::FFs_500.end());
//        _curPos = &(*curItr);
//        _curPos += 4096 + 500;
    } else {
        int actions_size = 24;                 // See recanalyst
        int rule_size = 16 + 16 * actions_size; // See recanalyst
        if (save_version_ > 11.9999)
            rule_size += 0x180;

        for (uint16_t i = 0, num_rules; i < 8; i++) {
            cursor_ >> 10 >> num_rules >> 4;
            if (num_rules > 10000)
                throw std::string("num_rules in AI data > 10000 (max is normally 10000).");
            for (int j = 0; j++ < num_rules; cursor_ >>= rule_size);
        }

        cursor_ >> 1448; // 104 + 320 + 1024 \note 这里我在104个字节后发现了2624个FF，也不知道为什么，应该是1344
        if (save_version_ >= 11.9599)
            cursor_ >> 1280; /// \todo 针对这个版本号边界要验证一下
        cursor_ >> 4096;     // 4096个00
    }

    if (save_version_ >= 12.2999 && IS_HD(version_code_)) {
        cursor_ >> 4; /// \todo 这里应该是 recanalyst 里的，需要验证。考虑到不确定性，最好加一段异常后尝试查找地图坐标位置的代码。
    }

    replay_start_ = cursor_();
}