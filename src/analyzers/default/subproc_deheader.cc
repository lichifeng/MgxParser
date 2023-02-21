/***************************************************************
 * \file       subproc_deheader.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"
#include "searcher.h"

void SkipStringBlock(RecCursor& cursor) {
    uint32_t crc;
    cursor >> crc;
    if (0 == crc || crc > 255) {
        cursor.ScanString();
        SkipStringBlock(cursor);
    }
}

void DefaultAnalyzer::AnalyzeDEHeader(int debugFlag) {
    if (!IS_DE(version_code_))
        return;

    status_.debug_flag_ = debugFlag;
    cursor_(version_end_);

    if (save_version_ >= 25.2199)
        cursor_ >> de_build_;
    if (save_version_ >= 26.1599)
        cursor_ >> de_timestamp_;
    cursor_ >> dd_version_
            >> dd_internal_version_
            >> dd_gameoptions_version_
            >> dd_dlccount_
            >> dd_dlccount_ * 4
            >> dd_dataset_ref_
            >> dd_difficulty_id_
            >> dd_selectedmap_id_
            >> dd_resolvedmap_id_
            >> revealmap_
            >> dd_victorytype_id_
            >> dd_startingresources_id_
            >> dd_startingage_id_
            >> dd_endingage_id_
            >> dd_gametype_
            >> 8
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
            >> dd_turbo_enabled_
            >> dd_shared_exploration_
            >> dd_team_positions_;
    if (save_version_ >= 13.3399)
        cursor_ >> dd_sub_gamemode_;
    if (save_version_ >= 13.3399)
        cursor_ >> dd_battle_royale_time_;
    if (save_version_ >= 25.0599)
        cursor_ >> dd_handicap_;

    uint8_t de_separator[] = {0xa3, 0x5f, 0x02, 0x00};
    if (!cursor_.TestAndSkip(de_separator, 4)) {
        throw std::string("Separator validation failed in DE header.");
    }

    // Read player data
    uint32_t total_players = save_version_ > 36.9999 ? dd_numplayers_ + 1 : 9; // actually 8 players
    for (size_t i = 1; i < total_players; i++) {
        cursor_ >> players[i].dd_dlc_id_
                >> players[i].dd_color_id_
                >> players[i].de_selected_color_
                >> players[i].de_selected_teamid_
                >> players[i].resolved_teamid_;
        cursor_.Hex(players[i].de_data_crc_, 8);
        cursor_ >> players[i].dd_mp_gameversion_
                >> players[i].dd_civ_id_
                >> players[i].dd_ai_type_
                >> players[i].dd_ai_civname_index_
                >> players[i].dd_ai_name_
                >> players[i].name
                >> players[i].type_;
        if (4 == players[i].type_)
            ++dd_ai_count_;
        cursor_ >> players[i].de_profile_id_
                >> 4 // Should be: 00 00 00 00
                >> players[i].dd_number_; // 不存在的话是 -1 FF FF FF FF
        if (save_version_ < 25.2199)
            cursor_ >> players[i].dd_rm_rating_;
        if (save_version_ < 25.2199)
            cursor_ >> players[i].dd_dm_rating_;
        cursor_ >> players[i].de_prefer_random_
                >> players[i].de_custom_ai_;
        if (save_version_ >= 25.0599)
            cursor_ >> 4 >> players[i].handicapping_level_;
    }
    cursor_ >> 9
            >> 1 //_readBytes(1, &DE_fogOfWar);
            >> dd_cheat_notifications_
            >> dd_colored_chat_;
    if (save_version_ > 36.9999) {
        cursor_ >> 4 * 3;
        cursor_.ScanString();
        cursor_ >> 1;
        cursor_.ScanString().ScanString();
        cursor_ >> (22 + 4 * 2 + 8);
    }
    cursor_ >> 4 // 0xa3, 0x5f, 0x02, 0x00
            >> dd_is_ranked_
            >> dd_allow_specs_
            >> dd_lobby_visibility_
            >> de_hidden_civs_
            >> de_matchmaking_;
    if (save_version_ >= 13.1299)
        cursor_ >> de_spec_dely_ >> de_scenario_civ_;

    // https://github.com/happyleavesaoc/aoc-mgz/blob/dd4d122259e3b97fe09ac08aa75fe7ff5a75c72d/mgz/header/de.py#L130
    SkipStringBlock(cursor_);
    cursor_ >> 8;
    for (size_t i = 0; i < 20; i++) {
        SkipStringBlock(cursor_);
    }
    if (save_version_ >= 25.2199) {
        int num_sn;
        cursor_ >> num_sn >> 4 * num_sn;
    } else {
        cursor_ >> 60 * 4;
    }
    cursor_ >> de_num_ai_files_;
    for (size_t i = 0; i < de_num_ai_files_; i++) {
        if (de_num_ai_files_ > 1000)
            throw std::string("Number of Ai files too large in DE header");
        cursor_ >> 4;
        cursor_.ScanString() >> 4;
    }
    if (save_version_ >= 25.0199)
        cursor_ >> 8;
    cursor_.Hex(dd_guid_, 16) >> dd_lobbyname_;
    if (save_version_ >= 25.2199)
        cursor_ >> 8;
    cursor_ >> dd_modded_dataset_ >> 19;
    if (save_version_ >= 13.1299)
        cursor_ >> 5;
    if (save_version_ >= 13.1699)
        cursor_ >> 9;
    if (save_version_ >= 20.0599)
        cursor_ >> 1;
    if (save_version_ >= 20.1599)
        cursor_ >> 8;
    if (save_version_ >= 25.0599)
        cursor_ >> 21;
    if (save_version_ >= 25.2199)
        cursor_ >> 4;
    if (save_version_ >= 26.1599)
        cursor_ >> 8;
    if (save_version_ >= 36.9999)
        cursor_ >> 3;
    cursor_.ScanString() >> 5;
    if (save_version_ >= 13.1299)
        cursor_ >> 1;
    if (save_version_ < 13.1699)
        cursor_.ScanString() >> 8; // uint32 + 00 00 00 00
    if (save_version_ >= 13.1699)
        cursor_ >> 2;
    if (save_version_ >= 36.9999)
        cursor_ >> 8;

    ai_start_ = cursor_();
}