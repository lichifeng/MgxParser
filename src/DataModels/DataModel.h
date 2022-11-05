/**
 * \file       DataModel.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-04
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#pragma once

#define UINT32_INIT 4294967295
#define FLOAT_INIT -20000.0
#define UINT8_INIT 255

#include "CompileConfig.h"

#include <string>
#include <map>
#include "nlohmann_json_3/json.hpp"
#include "Player.h"
#include "Helper.h"
#include "Chat.h"
#include "Lang/zh.h"

using namespace std;
using json = nlohmann::json;

class DataModel
{
public:
    /**
     * \brief      Read corresponding language string from a language file
     *
     * \param      l                   Language file
     * \param      i                   String index
     * \param      default             Fallback string if not found
     * \return     string              Translated string
     */
    string readLang(const map<uint32_t, string> &l, uint32_t i, string d = "-")
    {
        if (!(l.find(i) == l.end()))
        {
            return l.at(i);
        }
        else
        {
            return d;
        }
    }

    string toJson();

    // File-related members
    string filetype = "record"; ///< Or zip/rar/7z
    string extractedName;

    // Some config parameters
    uint16_t trailBytes = 5;               ///< 设定用于startinfo中玩家信息搜索时的特征字节长度，影响速度
    uint32_t easySkipBase = 35100;         ///< 在startinfo中搜索时可以放心跳过的字节长度

    // Version-related members
    uint32_t log_version_ = UINT32_INIT;     ///< body 的前4个字节，与版本有关，可以识别A/C版
    char version_string_[8] = {'\0'};      ///< 代表游戏版本的原始字符串
    float save_version_ = FLOAT_INIT;       ///< \warning float有精度，进行比较的时候注意要合理处理，比如>11.76要写成>11.7599这种
    VERSIONCODE version_code_ = UNDEFINED; ///< 这是自己定义的一个值，用于简化版本判断
    uint32_t includeAI;

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
    string HD_ver1000MapName;
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

    array<Player, 9> players;
    // uint8_t DE_fogOfWar;
    uint8_t DD_cheatNotifications;
    uint8_t DD_coloredChat;
    uint8_t DD_isRanked = 255;
    uint8_t DD_allowSpecs;
    uint32_t DD_lobbyVisibility;
    uint8_t DE_hiddenCivs;
    uint8_t DE_matchMaking;
    uint32_t DE_specDely;
    uint8_t DE_scenarioCiv;
    // string DE_RMSCrc;
    uint32_t HD_customRandomMapFileCrc;
    string HD_customScenarioOrCampaignFile;
    string HD_customRandomMapFile;
    string HD_customRandomMapScenarionFile;
    string DD_guid;
    string DD_lobbyName;
    string DD_moddedDataset;
    string HD_moddedDatasetWorkshopID;
    uint64_t DE_numAIFiles;

    // data from replay section
    uint32_t gameSpeed = UINT32_INIT; ///< \todo If de/hd, use data from de/hd-specific data
    uint16_t recPlayer; ///< \todo index or number of pov?? verify this.
    uint8_t numPlayers; ///< \todo gaia included, DD_numPlayers first??
    uint8_t instantBuild;
    uint8_t cheatsEnabled = 255;
    // uint16_t gameMode;     ///< \note Unknown.

    // data from map data section
    int32_t mapCoord[2];
    const void *mapdata_ptr_;
    uint8_t all_visible_;
    // uint8_t fogOfWar; ///< \note Use fogOfWar in lobby

    // data from start info
    uint32_t restore_time_;

    // data from scenario header
    float scenarioVersion = FLOAT_INIT;
    string scenarioFilename;
    string instructions;

    // victory conditions
    uint32_t victoryIsConquest;
    uint32_t victoryRelics;
    uint32_t victoryExplored;
    uint32_t victoryAnyOrAll;
    uint32_t victoryMode = UINT32_INIT;
    uint32_t victoryScore;
    uint32_t victoryTime;

    // game settings
    uint32_t mapID = UINT32_INIT;
    uint32_t difficultyID = UINT32_INIT;
    uint32_t lockTeams;

    // lobby settings
    uint32_t fogOfWar;
    uint32_t mapSize = UINT32_INIT;
    uint32_t population_limit_ = UINT32_INIT;
    uint8_t gameType;
    uint8_t lockDiplomacy; ///< \note DE/HD数据中还有一个类似的
    uint8_t treatyLength;  ///< \note DE/HD数据中还有一个类似的
    vector<Chat> chat;
    int32_t DE_mapSeed;

    // other data
    string embededMapName; ///< Map name extracted from instructions, not mapped with raw number

    // data from body
    uint32_t duration = 0;
    uint32_t isMultiplayer;
    uint32_t syncChecksumInterval = 500;

    string rawEncoding = RAW_ENCODING; // \note GB18030是为兼容性考虑。这里rawEncoding不能和outEncoding一样，有地方会用这两个相等来路过转换
    string outEncoding = OUT_ENCODING;
    // string playDate;                       ///< 游戏发生时间，对老录像只能推断 \todo
    ///< 有时需要从上传时间来推断，是否放在更外层的类里面？
    string message;                        ///< 对 \p status_old_ 的具体说明
    string parseMode = "Normal"; ///< 解析模式：normal, verbose, etc. 可以在命令行中指定
    double parseTime = 0;                  ///< 解析耗时（毫秒）

    string teamMode;
    string retroGuid;
};
