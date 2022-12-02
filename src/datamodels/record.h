/***************************************************************
 * \file       record.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#ifndef MGXPARSER_DATAMODEL_RECORD_H_
#define MGXPARSER_DATAMODEL_RECORD_H_

#define UINT32_INIT 4294967295
#define FLOAT_INIT -20000.0
#define UINT8_INIT 255

#include "compile_config.h"

#include <string>
#include <map>
#include "nlohmann_json_3/json.hpp"
#include "player.h"
#include "chat.h"
#include "lang/zh.h"
#include "auxiliary.h"

/**
 * dd_ prefixed members: exists in both HD/DE version
 * de_ prefixed members: exists in DE only
 * hd_ prefixed members: exists in HD only
 */
class Record {
public:
    // File-related members
    std::string input_ext_; ///< .zip/.mgx/.aoe2record/.mgz/.mgx2, etc.
    std::string extracted_file_; ///< filename of extracted record from .zip archive
    std::string file_md5_;

    // Version-related members
    uint32_t log_version_ = UINT32_INIT;     ///< body 的前4个字节，与版本有关，可以识别A/C版
    char version_string_[8] = {'\0'};      ///< 代表游戏版本的原始字符串
    float save_version_ = FLOAT_INIT;       ///< \warning float有精度，进行比较的时候注意要合理处理，比如>11.76要写成>11.7599这种
    VERSIONCODE version_code_ = UNDEFINED; ///< 这是自己定义的一个值，用于简化版本判断
    uint32_t include_ai_;

    // HD/DE-specific data from header stream
    uint32_t de_build_ = UINT32_INIT;           ///< In recent(2022-10) steam version of DE: 66692
    uint32_t de_timestamp_ = 0;                 ///< 游戏时间，只有DE版本中有
    float dd_version_ = FLOAT_INIT;             ///< hd<=4.7: 1000; hd=5.8: 1006
    uint32_t dd_internal_version_ = UINT32_INIT; ///< DE中是1000
    uint32_t dd_gameoptions_version_ = UINT32_INIT;
    uint32_t dd_dlccount_ = 0;
    uint32_t dd_dataset_ref_; ///< \todo What's this?
    uint32_t dd_difficulty_id_ = UINT32_INIT;
    uint32_t dd_selectedmap_id_;
    uint32_t dd_resolvedmap_id_;
    uint32_t revealmap_ = UINT32_INIT; ///< 0x00:通常，0x01:已开发，0x02:全部显示, 0x03:no fog
    uint32_t dd_victorytype_id_ = UINT32_INIT;
    uint32_t dd_startingresources_id_ = UINT32_INIT;
    uint32_t dd_startingage_id_;
    uint32_t dd_endingage_id_;
    uint32_t dd_gametype_ = UINT32_INIT; ///< Only when DD_version>=1006
    std::string hd_ver1000_mapname_;
    // string HD_ver1000Unknown;
    float dd_speed_ = FLOAT_INIT;
    uint32_t dd_treatylength_;
    // uint32_t DD_populationLimit = UINT32_INIT; ///< Store this value in populationLimit
    uint32_t dd_numplayers_ = UINT32_INIT; ///< \note Gaia not included
    uint32_t dd_unused_playercolor_;
    uint32_t dd_victoryamount_ = UINT32_INIT;
    uint8_t dd_trade_enabled_;
    uint8_t dd_teambonus_disabled_;
    uint8_t dd_random_positions_;
    uint8_t dd_alltechs_;
    uint8_t dd_num_startingunits_;
    uint8_t dd_lockteams_;
    uint8_t dd_lockspeed_;
    uint8_t dd_multiplayer_;
    uint8_t dd_cheats_ = 255;
    uint8_t dd_recordgame_;
    uint8_t dd_animals_enabled_;
    uint8_t dd_predators_enabled_;
    uint8_t dd_turbo_enabled_;
    uint8_t dd_shared_exploration_;
    uint8_t dd_team_positions_;
    uint32_t dd_sub_gamemode_;
    uint32_t dd_battle_royale_time_;
    uint8_t dd_handicap_ = UINT8_INIT; ///< 让分

    std::array<Player, 9> players;
    // uint8_t DE_fogOfWar;
    uint8_t dd_cheat_notifications_;
    uint8_t dd_colored_chat_;
    uint8_t dd_is_ranked_ = 255;
    uint8_t dd_allow_specs_;
    uint32_t dd_lobby_visibility_;
    uint8_t de_hidden_civs_;
    uint8_t de_matchmaking_;
    uint32_t de_spec_dely_;
    uint8_t de_scenario_civ_;
    // string DE_RMSCrc;
    uint32_t hd_custom_randommap_file_crc_;
    std::string hd_custom_scenario_or_campaign_file_;
    std::string hd_custom_randommap_file_;
    std::string hd_custom_randommap_scenarion_file_;
    std::string dd_guid_;
    std::string dd_lobbyname_;
    std::string dd_modded_dataset_;
    std::string hd_modded_dataset_workshop_id_;
    uint64_t de_num_ai_files_;

    // data from replay section
    uint32_t game_speed_ = UINT32_INIT; ///< \todo If de/hd, use data from de/hd-specific data
    uint16_t rec_player_; ///< \todo index or number of pov?? verify this.
    uint8_t num_players_; ///< \todo gaia included, DD_numPlayers first??
    uint8_t instant_build_;
    uint8_t cheats_enabled_ = 255;
    // uint16_t gameMode;     ///< \note Unknown.

    // data from map data section
    int32_t map_coord_[2];
    const void *mapdata_ptr_;
    uint8_t all_visible_;
    // uint8_t fogOfWar; ///< \note Use fogOfWar in lobby

    // data from start info
    uint32_t restore_time_;

    // data from scenario header
    float scenario_version_ = FLOAT_INIT;
    std::string scenario_filename_;
    std::string instructions;

    // victory conditions
    uint32_t victory_isconquest_;
    uint32_t victory_relics_;
    uint32_t victory_explored_;
    uint32_t victory_any_or_all_;
    uint32_t victory_mode_ = UINT32_INIT;
    uint32_t victory_score_;
    uint32_t victory_time_;

    // game settings
    uint32_t map_id_ = UINT32_INIT;
    uint32_t difficulty_id_ = UINT32_INIT;
    uint32_t lock_teams_;

    // lobby settings
    uint32_t fogofwar_;
    uint32_t map_size_ = UINT32_INIT;
    uint32_t population_limit_ = UINT32_INIT;
    uint8_t game_type_;
    uint8_t lock_diplomacy_; ///< \note DE/HD数据中还有一个类似的
    uint8_t treaty_length_;  ///< \note DE/HD数据中还有一个类似的
    std::vector<Chat> chat;
    int32_t de_mapseed_;

    // other data
    std::string embeded_mapname_; ///< Map name extracted from instructions, not mapped with raw number

    // data from body
    uint32_t duration_ = 0;
    uint32_t is_multiplayer_;
    uint32_t sync_checksum_interval_ = 500;

    std::string raw_encoding_ = RAW_ENCODING; // \note GB18030是为兼容性考虑
    std::string out_encoding_ = OUT_ENCODING;
    // string playDate;                       ///< \todo 游戏发生时间，对老录像只能推断
    ///< 有时需要从上传时间来推断，是否放在更外层的类里面？
    std::string message_;                        ///< 一些关于本次解析的其它说明
    std::string parse_mode_ = "Normal"; ///< 解析模式：normal, verbose, etc. 暂时没有用到
    double parse_time_ = 0;                  ///< 解析耗时（毫秒）

    std::string team_mode_;
    std::string retro_guid_;
};

#endif //MGXPARSER_DATAMODEL_RECORD_H_
