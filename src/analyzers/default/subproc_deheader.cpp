/**
 * \file       subProcHeaderDE.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"
#include "tools/searcher.h"

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
    for (size_t i = 1; i < 9; i++) {
        cursor_ >> players[i].dd_dlc_id_
                >> players[i].dd_color_id_
                >> players[i].de_selected_color_
                >> players[i].de_selected_teamid_
                >> players[i].resolved_teamid_;
        cursor_.BytesToHex(players[i].de_data_crc_, 8);
        cursor_ >> players[i].dd_mp_gameversion_
                >> players[i].dd_civ_id_
                >> players[i].dd_ai_type_
                >> players[i].DD_AICivNameIndex
                >> players[i].DD_AIName
                >> players[i].name
                >> players[i].type;
        if (4 == players[i].type)
            ++_DD_AICount;
        cursor_ >> players[i].DE_profileID
                >> 4 // Should be: 00 00 00 00
                >> players[i].DD_playerNumber; // 不存在的话是 -1 FF FF FF FF
        if (save_version_ < 25.2199)
            cursor_ >> players[i].DD_RMRating;
        if (save_version_ < 25.2199)
            cursor_ >> players[i].DD_DMRating;
        cursor_ >> players[i].DE_preferRandom
                >> players[i].DE_customAI;
        if (save_version_ >= 25.0599)
            cursor_ >> 4 >> players[i].handicappingLevel;
    }
    cursor_ >> 9
            >> 1 //_readBytes(1, &DE_fogOfWar);
            >> DD_cheatNotifications
            >> DD_coloredChat
            >> 4 // 0xa3, 0x5f, 0x02, 0x00
            >> DD_isRanked
            >> DD_allowSpecs
            >> DD_lobbyVisibility
            >> DE_hiddenCivs
            >> DE_matchMaking;
    if (save_version_ >= 13.1299)
        cursor_ >> DE_specDely >> DE_scenarioCiv;

    // if (saveVersion >= 13.1299)
    //     DE_RMSCrc = BytesToHex(_curPos, 4, true);

    /// \warning 实话我也不知道这一段是什么鬼东西，只好用搜索乱撞过去了.下面是做
    /// 的一些研究，找的规律
    /// \todo aoc-mgz有更新，可以参考
    /// https://github.com/happyleavesaoc/aoc-mgz/commit/4ffe9ad918b888531fc2e94c2b184cbd04ca9fb5
    /// \note de-13.03.aoe2record : 2a 00 00 00 fe ff ff ff + 59*(fe ff ff ff)
    /// de-13.06.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.07.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(fe ff ff ff)
    /// de-13.08.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(fe ff ff ff)
    /// de-13.13.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.15.aoe2record : 2A 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.17.aoe2record : 2C 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.20.aoe2record : 2C 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-13.34.aoe2record : 2D 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-20.06.aoe2record : 2D 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-20.16.aoe2record : 2D 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.01.aoe2record : 2E 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.02.aoe2record : 2E 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.06.aoe2record : 2F 00 00 00 FE FF FF FF + 59*(00 00 00 00)
    /// de-25.22.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    /// de-26.16.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    /// de-26.18.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    /// de-26.21.aoe2record : 2F 00 00 00 00 00 00 00 + NOTHING
    std::array<uint8_t, 2> hdstring_separator = {0x60, 0x0a};
    auto found = cursor_.Itr();
    auto header_end = cursor_.Itr(body_start_);
    for (size_t i = 0; i < 23; i++) {
        found = SearchPattern(
                found, header_end,
                hdstring_separator.begin(),
                hdstring_separator.end());
        if (found != header_end)
            found += (4 + *((uint16_t *) (&(*found) + 2)));
    }
    cursor_(found) >> 4; // 2a/c/d/e/f 00 00 00
    if (save_version_ >= 25.2199) {
        int tmp_len;
        cursor_ >> tmp_len >> 4 * tmp_len;
    } else {
        cursor_ >> 60 * 4;
    }
    cursor_ >> DE_numAIFiles;
    for (size_t i = 0; i < DE_numAIFiles; i++) {
        if (DE_numAIFiles > 1000)
            throw std::string("Number of Ai files too large in DE header");
        cursor_ >> 4;
        cursor_.ScanString() >> 4;
    }
    if (save_version_ >= 25.0199)
        cursor_ >> 8;
    cursor_.BytesToHex(DD_guid, 16) >> DD_lobbyName;
    if (save_version_ >= 25.2199)
        cursor_ >> 8;
    cursor_ >> DD_moddedDataset >> 19;
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
    cursor_.ScanString() >> 5;
    if (save_version_ >= 13.1299)
        cursor_ >> 1;
    if (save_version_ < 13.1699)
        cursor_.ScanString() >> 8; // uint32 + 00 00 00 00
    if (save_version_ >= 13.1699)
        cursor_ >> 2;

    ai_start_ = cursor_();
}