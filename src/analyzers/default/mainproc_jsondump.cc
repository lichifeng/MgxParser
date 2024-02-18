/***************************************************************
 * \file       mainproc_jsondump.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"
#include "nlohmann_json_3/json.hpp"
#include "auxiliary.h"
#include "lang/zh.h"
#include "lang/en.h"

using json = nlohmann::json;

// https://json.nlohmann.me/api/macros/nlohmann_json_serialize_enum/
NLOHMANN_JSON_SERIALIZE_ENUM
(VERSIONCODE,
 {
     { AOKTRIAL, "AOKTRIAL" },
     { AOCTRIAL, "AOCTRIAL" },
     { AOK, "AOK" },
     { AOC, "AOC" },
     { AOC10, "AOC10" },
     { AOC10C, "AOC10C" },
     { AOFE21, "AOFE21" },
     { USERPATCH12, "UP12" },
     { USERPATCH13, "UP13" },
     { USERPATCH14RC1, "UP14RC1" },
     { USERPATCH14RC2, "UP14RC2" },
     { USERPATCH14, "UP14" },
     { USERPATCH15, "UP15" },
     { MCP, "MCP" },
     { HD, "HD" },
     { HD43, "HD43" },
     { HD46_7, "HD46_7" },
     { HD48, "HD48" },
     { HD50_6, "HD50_6" },
     { HD57, "HD57" },
     { HD58, "HD58" },
     { DE, "DE" },
     { UNSUPPORTED, "UNSUPPORTED" },
     { UNDEFINED, "UNDEFINED" }
 });

std::string DefaultAnalyzer::JsonOutput(int indent) {
    json j;

    // Report
    j["message"] = message_;
    j["parseTime"] = parse_time_;
    j["parser"] = PARSER_VERSION_VERBOSE;
    j["status"] = status_.body_scanned_ ? "perfect" : status_.mapdata_found_ ? "good" : status_.stream_extracted_ ? "valid" : "invalid";
    j["filetype"] = input_ext_;
    j["filename"] = input_filename_;
    if (!modified_date_.empty())
        j["modifiedDate"] = modified_date_;
    if (!file_md5_.empty())
        j["filemd5"] = file_md5_;
    if (!extracted_file_.empty())
        j["extractedName"] = extracted_file_;
    if (status_.body_scanned_)
        j["duration"] = duration_;
    if (!dd_guid_.empty()) {
        j["guid"] = dd_guid_;
    } else if (!retro_guid_.empty()) {
        j["guid"] = retro_guid_;
    }
    j["recPlayer"] = rec_player_;
    if (status_.body_scanned_)
        j["isMultiplayer"] =
            (IS_DE(version_code_) || IS_HD(version_code_)) ? (bool)dd_multiplayer_ : (bool)is_multiplayer_;
    if (de_timestamp_)
        j["gameTime"] = de_timestamp_;

    // Version info
    if (status_.version_detected_)
        j["version"]["code"] = version_code_;

    if (UINT32_INIT != log_version_)
        j["version"]["logVer"] = log_version_;

    if ('\0' != version_string_[0])
        j["version"]["rawStr"] = version_string_;

    if (FLOAT_INIT != save_version_)
        j["version"]["saveVer"] = save_version_;

    if (FLOAT_INIT != scenario_version_)
        j["version"]["scenarioVersion"] = scenario_version_;

    if (FLOAT_INIT != dd_version_)
        j["version"]["newVer"] = dd_version_;

    if (UINT32_INIT != dd_internal_version_)
        j["version"]["interVer"] = dd_internal_version_;

    if (UINT32_INIT != de_build_)
        j["version"]["build"] = de_build_;

    // Instruction
    if (!instructions.empty())
        j["instruction"] = instructions;

    // Settings
    if (status_.encoding_detected_)
        j["rawEncoding"] = raw_encoding_;
    if (FLOAT_INIT != dd_speed_ || UINT32_INIT != game_speed_) {
        j["speed"] = Translate(zh::kSpeed, FLOAT_INIT == dd_speed_ ? game_speed_ : (uint32_t) (dd_speed_ * 1000));
        j["speedEn"] = Translate(en::kSpeed, FLOAT_INIT == dd_speed_ ? game_speed_ : (uint32_t) (dd_speed_ * 1000));
    }
    if (UINT32_INIT != dd_victorytype_id_) {
        j["victory"]["type"] = Translate(zh::kVictoryTypes, dd_victorytype_id_);
        j["victory"]["typeEn"] = Translate(en::kVictoryTypes, dd_victorytype_id_);
    }
    else if (UINT32_INIT != victory_mode_) {
        j["victory"]["type"] = Translate(zh::kVictoryTypes, victory_mode_); // \todo 低版本的要核实下，好像不怎么对
        j["victory"]["typeEn"] = Translate(en::kVictoryTypes, victory_mode_);
    }

    if (UINT32_INIT != population_limit_)
        j["population"] = population_limit_;

    if (!team_mode_.empty())
        j["teamMode"] = team_mode_;

    j["includeAI"] = (bool) include_ai_;

    // Map
    if (UINT32_INIT != map_size_) {
        j["map"]["size"] = Translate(zh::kMapSize, map_size_);
        j["map"]["sizeEn"] = Translate(en::kMapSize, map_size_);
    }
    if (embeded_mapname_.empty()) {
        j["map"]["name"] = Translate(IS_DE(version_code_) ? zh::kDEMapNames : zh::kMapNames, dd_resolvedmap_id_ == UINT32_INIT ? map_id_ : dd_resolvedmap_id_);
        j["map"]["nameEn"] = Translate(IS_DE(version_code_) ? en::kDEMapNames : en::kMapNames, dd_resolvedmap_id_ == UINT32_INIT ? map_id_ : dd_resolvedmap_id_);
    } else {
        j["map"]["name"] = embeded_mapname_;
    }
    try {
        if (map_type_ == MgxParser::BASE64_NORMAL) {
            j["map"]["base64"] = DrawMap(300, 150);
        } else if (map_type_ == MgxParser::BASE64_HD) {
            j["map"]["base64"] = DrawMap(900, 450);
        }
    } catch (...) {
        // ...
    }
    

    // Chat
    for (auto &c: chat) {
        json cj;

        cj["time"] = c.time;
        cj["msg"] = c.msg;

        j["chat"].emplace_back(cj);
    }

    // Players
    for (auto &p: players) {
        if (!p.Valid())
            continue;
        json pj;

        pj["index"] = p.index;
        pj["slot"] = p.slot;
        pj["name"] = p.dd_ai_type_.empty() ? p.name : (p.dd_ai_name_.empty() ? p.dd_ai_type_ : p.dd_ai_name_);
        pj["team"] = 1 == p.resolved_teamid_ ? 10 + p.index : p.resolved_teamid_;
        pj["civilization"]["id"] = (UINT32_INIT == p.dd_civ_id_) ? p.civ_id_ : p.dd_civ_id_;
        pj["civilization"]["name"] = Translate(zh::kCivNames, pj["civilization"]["id"]);
        pj["civilization"]["nameEn"] = Translate(en::kCivNames, pj["civilization"]["id"]);
        pj["initPosition"] = {
            p.init_camera_[0] == -1.0 ? 0 : p.init_camera_[0],
            p.init_camera_[1] == -1.0 ? 0 : p.init_camera_[1]
        };

        if (4 == p.type_ && !p.dd_ai_type_.empty()) {
            pj["type"] = Translate(zh::kPlayerTypes, p.type_) + "(" + p.dd_ai_type_ + ")";
            pj["typeEn"] = Translate(en::kPlayerTypes, p.type_) + "(" + p.dd_ai_type_ + ")";
        } else {
            pj["type"] = Translate(zh::kPlayerTypes, p.type_);
            pj["typeEn"] = Translate(en::kPlayerTypes, p.type_);
        }

        if (0 != p.de_profile_id_)
            pj["DEProfileID"] = p.de_profile_id_;
        if (0 != p.hd_steam_id_)
            pj["HDSteamID"] = p.hd_steam_id_;
        pj["mainOp"] = p.InitialDataFound();
        if (UINT32_INIT != p.handicapping_level_)
            pj["handicappingLevel"] = p.handicapping_level_;
        if (-1 != p.resigned_)
            pj["resigned"] = p.resigned_;
        if (-1 != p.feudal_time_)
            pj["feudalTime"] = p.feudal_time_;
        if (-1 != p.castle_time_)
            pj["castleTime"] = p.castle_time_;
        if (-1 != p.imperial_time_)
            pj["imperialTime"] = p.imperial_time_;
        pj["disconnected"] = p.disconnected_;
        pj["isWinner"] = p.is_winner_;
        pj["colorIndex"] = (UINT32_INIT == p.dd_color_id_) ? (int)p.color_id_ : p.dd_color_id_;

        j["players"].emplace_back(std::move(pj));
    }

    return j.dump(indent, ' ', false, json::error_handler_t::ignore);
}