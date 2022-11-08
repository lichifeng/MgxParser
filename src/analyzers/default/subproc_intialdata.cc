/***************************************************************
 * \file       subproc_intialdata.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeInitialData(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    if (!cursor_(initinfo_start_)())
        return;

    uint32_t num_headerdata = *(uint32_t *) (initinfo_searchpattern_trail_ + 1);

    for (auto &p: players) {
        if (p.InitialDataFound() && p.Valid()) {
            cursor_(p.data_offset_).ScanString() >> 762;

            if (save_version_ >= 11.7599)
                cursor_ >> 36;
            if (IS_DE(version_code_) || IS_HD(version_code_))
                cursor_ >> (4 * (num_headerdata - 198));
            if (version_code_ == USERPATCH15 || version_code_ == MCP)
                cursor_ >> p.modversion_id_ >> (4 * 6 + 4 * 7 + 4 * 28);
            if (version_code_ == MCP)
                cursor_ >> (4 * 65);
            cursor_ >> 1 >> p.init_camera_;

            // Do some check here
            if (p.init_camera_[0] < 0 || p.init_camera_[0] > map_coord_[0] || p.init_camera_[1] < 0 ||
                p.init_camera_[1] > map_coord_[1]) {
                p.init_camera_[0] = p.init_camera_[1] = 0;
                continue;
            }

            if (!IS_AOK(version_code_)) {
                int32_t num_savedviews;
                cursor_ >> num_savedviews;
                if (num_savedviews > 0)
                    cursor_ >> 8 * num_savedviews;
            }

            cursor_ >> 5 >> p.civ_id_ >> 3 >> p.color_id_;
        }
    }
}