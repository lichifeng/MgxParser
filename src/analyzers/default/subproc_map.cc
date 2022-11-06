/**
 * \file       subProcMapData.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeMap(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(map_start_) >> mapCoord;
    if (mapCoord[0] >= 10000 || mapCoord[1] >= 10000) {
        throw std::string("Map size too large.");
    } else if (mapCoord[0] != mapCoord[1]) {
        throw std::string("Map coordinates is weird.");
    }

    int32_t num_mapzones, map_bits, num_floats;
    map_bits = mapCoord[0] * mapCoord[1];
    cursor_ >> num_mapzones;
    for (int i = 0; i < num_mapzones; i++) {
        if (IS_HD(version_code_) || IS_DE(version_code_)) /// \todo 为什么不是11.76？
            cursor_ >> (2048 + map_bits * 2);
        else
            cursor_ >> (1275 + map_bits);
        cursor_ >> num_floats >> (num_floats * 4 + 4);
    }
    cursor_ >> all_visible_ >> 1; // fog of war

    mapdata_ptr_ = cursor_.Ptr();

    uint32_t check_val = *(uint32_t *) (cursor_.Ptr() + 7 * map_bits);
    if (IS_DE(version_code_)) {
        maptile_type_ = (save_version_ >= 13.0299 || check_val > 1000) ? 9 : 7;
    } else {
        maptile_type_ = (cursor_.Ptr()[0] == 255) ? 4 : 2;
    }
    cursor_ >> (map_bits * maptile_type_);

    int32_t num_data, num_obstructions;
    cursor_ >> num_data >> (4 + num_data * 4);
    for (int i = 0; i < num_data; i++) {
        cursor_ >> num_obstructions >> (num_obstructions * 8);
    }
    int32_t visibilityMapSize[2];
    cursor_ >> visibilityMapSize >> (visibilityMapSize[0] * visibilityMapSize[1] * 4);

    // Find start point of init info
    cursor_ >> restore_time_;

    uint32_t num_particles;
    cursor_ >> num_particles >> (27 * num_particles);

    // A checkpoint, expecting 10060 with AOK and 40600 with higher version
    // borrow local var check_val
    cursor_ >> check_val;
    if (40600 != check_val && 10060 != check_val) // 10060 in AOK
        throw std::string("Cannot find expected check value 10060/40600 in init info section.");

    initinfo_start_ = cursor_();
    status_.mapdata_found_ = true;
}