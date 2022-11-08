/***************************************************************
 * \file       subproc_skiptriggers.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::SkipTriggers(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(trigger_start_) >> 1; // 1 is zero

    int32_t num_triggers,
            description_len,
            name_len,
            num_effects,
            num_selected_objs,
            text_len,
            sound_filename_len,
            num_conditions;
    bool is_hdpatch4 = save_version_ >= 12.3399;
    int condition_size = is_hdpatch4 ? 80 : 72;

    cursor_ >> num_triggers;
    for (int i = 0; i < num_triggers; i++) {
        cursor_ >> 18
                >> description_len
                >> (description_len > 0 ? description_len : 0)
                >> name_len
                >> (name_len > 0 ? name_len : 0)
                >> num_effects;
        for (int j = 0; j < num_effects; j++) {
            cursor_ >> 24
                    >> num_selected_objs;
            num_selected_objs = num_selected_objs == -1 ? 0 : num_selected_objs;
            cursor_ >> (is_hdpatch4 ? 76 : 72)
                    >> text_len
                    >> (text_len > 0 ? text_len : 0)
                    >> sound_filename_len
                    >> (sound_filename_len > 0 ? sound_filename_len : 0)
                    >> num_selected_objs
                    >> (4 * num_selected_objs);
        }
        cursor_ >> (4 * num_effects)
                >> num_conditions
                >> (num_conditions * condition_size + num_conditions * 4);
    }

    if (num_triggers > 0)
        cursor_ >> 4 * num_triggers;

    if (IS_DE(version_code_))
        cursor_ >> 1032;

    lobby_start_ = cursor_();
}