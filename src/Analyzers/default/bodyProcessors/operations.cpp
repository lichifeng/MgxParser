/**
 * \file       bodyProcessors.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-04
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include <string>
#include "../Analyzer.h"
#include "nlohmann_json_3/json.hpp"

using json = nlohmann::json;

uint32_t syncData[2];
Chat tmpChat;
uint32_t cmdLen;
const uint8_t *pNext;
uint8_t cmd;
int tmpIndex;

void DefaultAnalyzer::_handleOpSync()
{
    _readBytes(8, syncData);
    duration += syncData[0];
    //_skip(_bodyVar.syncIndex == _bodyVar.syncChecksumInterval ? 28 : 0);
    _skip(syncData[1] == 0 ? 28 : 0);
    _skip(12); // \todo 这里的信息可能有用，包含了视角的的坐标
}

void DefaultAnalyzer::_handleOpViewlock()
{
    _skip(12); // TODO: 看起来是视角切换的命令，比如按H切到TC这种(4byte float, 4byte float, 4byte int)
}

void DefaultAnalyzer::_handleOpChat()
{
    if (*(int32_t *)_curPos != -1)
    {
        return;
    }
    else
    {
        _skip(4);
    }
    _readPascalString(tmpChat.msg, true, true);
    tmpIndex = tmpChat.msg[2] - '0';
    // Two types of messages: "@#8... ..." in earlier versions and json strings
    // in DE version
    if ('@' == tmpChat.msg[0] && '#' == tmpChat.msg[1])
    {
        // Filter junk info like being attacked by animals, etc.
        if (tmpIndex > 0 && tmpIndex < 9 && 0 != tmpChat.msg.compare(3, players[tmpIndex].name.size(), players[tmpIndex].name) || tmpIndex == 0)
            return;

        if ('\0' == tmpChat.msg.back() && tmpChat.msg.size() > 0)
            tmpChat.msg.resize(tmpChat.msg.size() - 1);
    }
    else
    {
        try
        {
            auto j = json::parse(tmpChat.msg);
            tmpChat.msg = j.at("messageAGP");
        }
        catch (const exception &e)
        {
            logger->fatal("Json Parse Exception@{}: {}", _debugFlag, e.what());
            _sendExceptionSignal();
        }
    }

    tmpChat.time = duration;
    chat.emplace_back(tmpChat);
}

void DefaultAnalyzer::_handleOpCommand()
{
    // \todo Merge de71094 update from https://github.com/happyleavesaoc/aoc-mgz/commit/19b75b3b7e51e254075644ea2be96fdb0a7477b1 
    _readBytes(4, &cmdLen);

    pNext = _remainBytes() < cmdLen ? _curStream->data() + _curStream->size() : _curPos + cmdLen;
    //_readBytes(1, &cmd);

    _handleAction();

    _curPos = pNext;
}
