/***************************************************************
 * \file       subproc_finddisabledtechs.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::FindDisabledTechs(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    if (!IS_DE(version_code_)) {
        if (IS_HD(version_code_) && save_version_ < 12.3399 && save_version_ > 11.9701) {
            disabledtechs_start_ = gamesettings_start_ - 5396;
        } else {
            disabledtechs_start_ = gamesettings_start_ - 5392;
        }
    } else {
        disabledtechs_start_ = gamesettings_start_ - 276;
    }

    if (IS_HD(version_code_) && save_version_ >= 12.3399)
        disabledtechs_start_ -= 644;

    // Check if at correct point
    if (cursor_(disabledtechs_start_).Peek<uint32_t>() != -99) {
        cursor_ -= 4;                     // 先后退四个字节再开始查找

        for (size_t i = 0; i < 8000; i++) { // \todo 这个8000随便定的，.mgx2文件里好像差几个字节，不管了，反正特殊情况就查找吧
            if (cursor_.Peek<uint32_t>() == -99) {
                disabledtechs_start_ = cursor_();
                return;
            }
            cursor_ -= 1;
        }

        throw std::string("Failed to find disabled techs section.");
    }
}