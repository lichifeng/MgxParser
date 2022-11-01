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

string DefaultAnalyzer::toJson()
{
    json j;

    // Decide whether to ditch this file
    j["isRecfile"] = UINT32_INIT != logVersion;

    // Version info
    j["version"]["code"] = version_code_;

    if (UINT32_INIT != logVersion)
        j["version"]["logVer"] = logVersion;

    if ('\0' != versionStr[0])
        j["version"]["rawStr"] = versionStr;

    if (FLOAT_INIT != saveVersion)
        j["version"]["saveVer"] = saveVersion;

    if (FLOAT_INIT != scenarioVersion)
        j["version"]["scenarioVersion"] = scenarioVersion;

    if (FLOAT_INIT != DD_version)
        j["version"]["newVer"] = DD_version;

    if (UINT32_INIT != DD_internalVersion)
        j["version"]["interVer"] = DD_internalVersion;

    if (UINT32_INIT != DE_build)
        j["version"]["build"] = DE_build;

    // Instruction
    if (!instructions.empty())
        j["instruction"] = instructions;

    // Settings
    j["fileType"] = filetype;
    j["filename"] = input_filename_;
    if (!extractedName.empty())
        j["extractedName"] = extractedName;
    j["rawEncoding"] = rawEncoding;
    j["speed"] = readLang(zh::speed, FLOAT_INIT == DD_speed ? gameSpeed : (uint32_t)(DD_speed * 1000));
    if (UINT32_INIT != DD_victoryTypeID)
        j["victory"]["type"] = readLang(zh::victoryTypes, DD_victoryTypeID);
    else if (UINT32_INIT != victoryMode)
        j["victory"]["type"] = readLang(zh::victoryTypes, victoryMode); // \todo 低版本的要核实下，好像不怎么对

    if (UINT32_INIT != populationLimit)
        j["population"] = populationLimit;

    if (!teamMode.empty())
        j["teamMode"] = teamMode;

    j["includeAI"] = (bool)includeAI;

    // Map
    if (UINT32_INIT != mapSize)
        j["map"]["size"] = readLang(zh::mapSize, mapSize);

    // Report
    j["status"] = status_old_;
    j["duration"] = duration;
    j["message"] = message;
    if (!DD_guid.empty())
    {
        j["guid"] = DD_guid;
    }
    else if (!retroGuid.empty())
    {
        j["guid"] = retroGuid;
    }
    j["parser"] = PARSER_VERSION_VERBOSE;

    // Players
    for (auto &p : players)
    {
        if (!p.valid())
            continue;
        json pJ;

        pJ["index"] = p.index;
        pJ["slot"] = p.slot;
        pJ["name"] = p.DD_AIType.empty() ? p.name : p.DD_AIName;
        pJ["team"] = 1 == p.resolvedTeamID ? 10 + p.index : p.resolvedTeamID;
        pJ["civilization"]["id"] = (UINT32_INIT == p.DD_civID) ? p.civID : p.DD_civID;
        pJ["civilization"]["name"] = readLang(zh::civNames, pJ["civilization"]["id"]);
        pJ["initPosition"] = {
            p.initCamera[0] == -1.0 ? 0 : p.initCamera[0],
            p.initCamera[1] == -1.0 ? 0 : p.initCamera[1]};

        if (4 == p.type && !p.DD_AIType.empty())
            pJ["type"] = readLang(zh::playerTypes, p.type) + "(" + p.DD_AIType + ")";
        else
            pJ["type"] = readLang(zh::playerTypes, p.type);

        if (0 != p.DE_profileID)
            pJ["DEProfileID"] = p.DE_profileID;
        if (0 != p.HD_steamID)
            pJ["HDSteamID"] = p.HD_steamID;
        pJ["mainOp"] = p.initialDataFound() ? true : false; // \todo 要验证下。可以用这种方法确定是不是Co-Op。
        pJ["POV"] = p.slot == recPlayer;
        if (UINT32_INIT != p.handicappingLevel)
            pJ["handicappingLevel"] = p.handicappingLevel;
        if (-1 != p.resigned)
            pJ["resigned"] = p.resigned;
        if (-1 != p.feudalTime)
            pJ["feudalTime"] = p.feudalTime;
        if (-1 != p.castleTime)
            pJ["castleTime"] = p.castleTime;
        if (-1 != p.imperialTime)
            pJ["imperialTime"] = p.imperialTime;
        pJ["disconnected"] = p.disconnected;
        pJ["inWinner"] = p.isWinner;

        j["players"].emplace_back(pJ);
    }

    j["parseTime"] = parseTime;

    return j.dump(-1, ' ', false, json::error_handler_t::ignore);
}