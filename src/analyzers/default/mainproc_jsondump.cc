/**
 * \file       datamodel.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-05
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

string DefaultAnalyzer::toJson() {
    json j;

    // Decide whether to ditch this file
    j["isRecfile"] = UINT32_INIT != log_version_;

    // Version info
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
    j["fileType"] = input_ext_;
    j["filename"] = input_filename_;
    if (!extracted_file_.empty())
        j["extractedName"] = extracted_file_;
    j["rawEncoding"] = raw_encoding_;
    j["speed"] = readLang(zh::speed, FLOAT_INIT == dd_speed_ ? gamespeed_ : (uint32_t) (dd_speed_ * 1000));
    if (UINT32_INIT != dd_victorytype_id_)
        j["victory"]["type"] = readLang(zh::victoryTypes, dd_victorytype_id_);
    else if (UINT32_INIT != victoryMode)
        j["victory"]["type"] = readLang(zh::victoryTypes, victoryMode); // \todo 低版本的要核实下，好像不怎么对

    if (UINT32_INIT != population_limit_)
        j["population"] = population_limit_;

    if (!teamMode.empty())
        j["teamMode"] = teamMode;

    j["includeAI"] = (bool) includeAI;

    // Map
    if (UINT32_INIT != map_size_)
        j["map"]["size"] = readLang(zh::mapSize, map_size_);

    // Report
    j["status"] = status_old_;
    j["duration"] = duration_;
    j["message"] = message;
    if (!DD_guid.empty()) {
        j["guid"] = DD_guid;
    } else if (!retro_guid_.empty()) {
        j["guid"] = retro_guid_;
    }
    j["parser"] = PARSER_VERSION_VERBOSE;

    // Players
    for (auto &p: players) {
        if (!p.Valid())
            continue;
        json pJ;

        pJ["index"] = p.index;
        pJ["slot"] = p.slot;
        pJ["name"] = p.dd_ai_type_.empty() ? p.name : p.DD_AIName;
        pJ["team"] = 1 == p.resolved_teamid_ ? 10 + p.index : p.resolved_teamid_;
        pJ["civilization"]["id"] = (UINT32_INIT == p.dd_civ_id_) ? p.civ_id_ : p.dd_civ_id_;
        pJ["civilization"]["name"] = readLang(zh::civNames, pJ["civilization"]["id"]);
        pJ["initPosition"] = {
                p.init_camera_[0] == -1.0 ? 0 : p.init_camera_[0],
                p.init_camera_[1] == -1.0 ? 0 : p.init_camera_[1]};

        if (4 == p.type_ && !p.dd_ai_type_.empty())
            pJ["type"] = readLang(zh::playerTypes, p.type_) + "(" + p.dd_ai_type_ + ")";
        else
            pJ["type"] = readLang(zh::playerTypes, p.type_);

        if (0 != p.DE_profileID)
            pJ["DEProfileID"] = p.DE_profileID;
        if (0 != p.HD_steamID)
            pJ["HDSteamID"] = p.HD_steamID;
        pJ["mainOp"] = p.InitialDataFound(); // \todo 要验证下。可以用这种方法确定是不是Co-Op。
        pJ["POV"] = p.slot == rec_player_;
        if (UINT32_INIT != p.handicappingLevel)
            pJ["handicappingLevel"] = p.handicappingLevel;
        if (-1 != p.resigned_)
            pJ["resigned"] = p.resigned_;
        if (-1 != p.feudal_time_)
            pJ["feudalTime"] = p.feudal_time_;
        if (-1 != p.castle_time_)
            pJ["castleTime"] = p.castle_time_;
        if (-1 != p.imperial_time_)
            pJ["imperialTime"] = p.imperial_time_;
        pJ["disconnected"] = p.disconnected_;
        pJ["inWinner"] = p.is_winner_;

        j["players"].emplace_back(pJ);
    }

    j["parseTime"] = parseTime;

    return j.dump(-1, ' ', false, json::error_handler_t::ignore);
}