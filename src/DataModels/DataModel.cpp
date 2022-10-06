/**
 * \file       DataModel.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-05
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "DataModel.h"

string DataModel::toJson()
{
    json j;

    // \todo 要对所有字符串的默认值进行一个处理，不然会遇到[json.exception.type_error.316] invalid UTF-8 byte at index 0: 0x90
    j["filename"] = filename;
    j["originSize"] = filesize;
    j["parser"] = PARSER_VERSION;
    j["parseMode"] = parseMode;
    j["parseTime"] = parseTime;
    j["rawEncoding"] = rawEncoding;
    j["outEncoding"] = outEncoding;
    j["status"] = status; // \todo 是否要放到执行完再改成GOOD？？
    j["message"] = message;

    j["version"] = {
        {"code", versionCode},
        {"rawStr", versionStr},
        {"logVer", logVersion},
        {"saveVer", saveVersion}};
    if (FLOAT_INIT == scenarioVersion)
        j["version"]["scenarioVersion"] = nullptr;
    else
        j["version"]["scenarioVersion"] = scenarioVersion;

    if (FLOAT_INIT == DD_version)
        j["version"]["newVer"] = nullptr;
    else
        j["version"]["newVer"] = DD_version;

    if (UINT32_INIT == DD_internalVersion)
        j["version"]["interVer"] = nullptr;
    else
        j["version"]["interVer"] = DD_internalVersion;

    if (UINT32_INIT == DE_build)
        j["version"]["build"] = nullptr;
    else
        j["version"]["build"] = DE_build;

    // victory conditions
    if (UINT32_INIT != DD_victoryTypeID)
    {
        j["victory"]["type"] = readLang(zh::victoryTypes, DD_victoryTypeID);
        if (7 == DD_victoryTypeID) // 时限
        {
            j["victory"]["amount"] = readLang(zh::victoryAmount, DD_victoryAmount);
        }
        else
        {
            j["victory"]["amount"] = DD_victoryAmount;
        }
    } else {
        j["victory"]["type"] = readLang(zh::victoryTypes, victoryMode); // \todo 低版本的要核实下，好像不怎么对
    }

    if (DE_timestamp)
        j["playTime"] = DE_timestamp;
    else
        j["playTime"] = nullptr;

    j["includeAI"] = (bool)includeAI;
    j["duration"] = duration;
    j["instruction"] = instructions;

    if (!scenarioFilename.empty())
        j["scenarioFilename"] = scenarioFilename;
    else
        j["scenarioFilename"] = nullptr;

    for (auto &p : players)
    {
        if (!p.valid())
            continue;
        json pJ;

        pJ["index"] = p.index;
        pJ["slot"] = p.slot;
        pJ["name"] = p.name;
        pJ["civilization"]["id"] = (UINT32_INIT == p.DD_civID) ? p.civID : p.DD_civID;
        pJ["civilization"]["name"] = {readLang(zh::civNames, pJ["civilization"]["id"])};
        pJ["initPosition"] = {
            p.initCamera[0] == -1.0 ? 0 : p.initCamera[0],
            p.initCamera[1] == -1.0 ? 0 : p.initCamera[1]};
        pJ["type"] = readLang(zh::playerTypes, p.type);
        if (0 != p.DE_profileID)
        {
            pJ["DEProfileID"] = p.DE_profileID;
        }
        else
        {
            pJ["DEProfileID"] = nullptr;
        }
        if (0 != p.HD_steamID)
        {
            pJ["HDSteamID"] = p.HD_steamID;
        }
        else
        {
            pJ["HDSteamID"] = nullptr;
        }
        pJ["mainOp"] = p.initialDataFound() ? true : false; // \todo 要验证下。可以用这种方法确定是不是Co-Op。

        j["players"].emplace_back(pJ);
    }

    for (auto &c : chat)
    {
        j["chat"].emplace_back(json{
            {"time", c.time},
            {"msg", c.msg}});
    }

    return j.dump();
}