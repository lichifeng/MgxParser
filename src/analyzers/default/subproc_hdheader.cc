/***************************************************************
 * \file       subproc_hdheader.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeHDHeader(int debugFlag) {
    if (!(IS_HD(version_code_) && save_version_ > 12.3401)) // \todo is this right cutoff point? Test .mgx2 file
        return;

    status_.debug_flag_ = debugFlag;
    cursor_(version_end_);

    cursor_ >> dd_version_;
    if (dd_version_ >= 1005.9999)
        version_code_ = HD57;
    cursor_ >> dd_internal_version_
            >> dd_gameoptions_version_
            >> dd_dlccount_
            >> 4 * dd_dlccount_
            >> dd_dataset_ref_
            >> dd_difficulty_id_
            >> dd_selectedmap_id_
            >> dd_resolvedmap_id_
            >> revealmap_
            >> dd_victorytype_id_
            >> dd_startingresources_id_
            >> dd_startingage_id_
            >> dd_endingage_id_;
    if (dd_version_ >= 1005.9999)
        cursor_ >> dd_gametype_;

    uint8_t de_separator[] = {0xa3, 0x5f, 0x02, 0x00};
    if (!cursor_.TestAndSkip(de_separator, 4)) {
        throw std::string("Separator validation failed in HD header.");
    }

    if (dd_version_ == 1000) {
        cursor_ >> hd_ver1000_mapname_;
        cursor_.ScanString();
    }
    // Next 4 bytes should be: a3 5f 02 00
    cursor_ >> 4
            >> dd_speed_
            >> dd_treatylength_
            >> population_limit_
            >> dd_numplayers_
            >> dd_unused_playercolor_
            >> dd_victoryamount_
            >> 4
            >> dd_trade_enabled_
            >> dd_teambonus_disabled_
            >> dd_random_positions_
            >> dd_alltechs_
            >> dd_num_startingunits_
            >> dd_lockteams_
            >> dd_lockspeed_
            >> dd_multiplayer_
            >> dd_cheats_
            >> dd_recordgame_
            >> dd_animals_enabled_
            >> dd_predators_enabled_
            >> 4;
    if (dd_version_ == 1000) {
        cursor_ >> (120 + 4 + 40);
        for (size_t i = 0; i < 8; i++) {
            cursor_.ScanString();
        }
        cursor_ >> (16 + 4 + 10);
    } else {
        // Check if HD version is between [5.0, 5.7]
        int32_t check, test;
        auto tmp_pos = cursor_();
        cursor_ >> check >> 4;
        if (dd_version_ >= 1005.9999)
            cursor_ >> 1;
        cursor_ >> 15;
        cursor_.ScanString();
        cursor_ >> 1;
        if (dd_version_ >= 1004.9999)
            cursor_.ScanString();
        cursor_.ScanString()
                >> 16
                >> test;
        if (check != test)
            version_code_ = HD58;
        cursor_(tmp_pos);

        // Read player data
        for (size_t i = 1; i < 9; i++) {
            cursor_ >> players[i].dd_dlc_id_
                    >> players[i].dd_color_id_;
            if (dd_version_ >= 1005.9999)
                cursor_ >> 1;
            cursor_ >> 2
                    >> players[i].hd_data_crc_
                    >> players[i].dd_mp_gameversion_
                    >> 4 // Not correct team index
                    >> players[i].dd_civ_id_
                    >> players[i].dd_ai_type_
                    >> players[i].dd_ai_civname_index_;
            if (dd_version_ >= 1004.9999)
                cursor_ >> players[i].dd_ai_name_;
            cursor_ >> players[i].name
                    >> players[i].type_
                    >> players[i].hd_steam_id_
                    >> players[i].dd_number_;
            if (4 == players[i].type_)
                ++dd_ai_count_;
            if (dd_version_ >= 1005.9999 && version_code_ != HD57) // \todo test this
                cursor_ >> players[i].dd_rm_rating_ >> players[i].dd_dm_rating_;
        }

        cursor_ >> 1 // fog of war
                >> dd_cheat_notifications_
                >> dd_colored_chat_
                >> 13 // 9 bytes + a3 5f 02 00
                >> dd_is_ranked_
                >> dd_allow_specs_
                >> dd_lobby_visibility_
                >> hd_custom_randommap_file_crc_
                >> hd_custom_scenario_or_campaign_file_
                >> 8
                >> hd_custom_randommap_file_
                >> 8
                >> hd_custom_randommap_scenarion_file_
                >> 8;
        cursor_.Hex(dd_guid_, 16)
                >> dd_lobbyname_
                >> dd_modded_dataset_;
        cursor_.Hex(hd_modded_dataset_workshop_id_, 4);
        if (dd_version_ >= 1004.9999) {
            cursor_ >> 4;
            cursor_.ScanString();
            cursor_ >> 4;
        }
    }

    ai_start_ = cursor_();
}