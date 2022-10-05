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

uint32_t syncData[2];
Chat tmpChat;
uint32_t cmdLen;
uint8_t *pNext;
uint8_t cmd;

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
    if ('\0' == tmpChat.msg.back())
        tmpChat.msg.resize(tmpChat.msg.size() - 1);
    tmpChat.time = duration;
    chat.emplace_back(tmpChat);
}

void DefaultAnalyzer::_handleOpCommand()
{
    _readBytes(4, &cmdLen);

    pNext = _remainBytes() < cmdLen ? _curStream->data() + _curStream->size() : _curPos + cmdLen;
    _readBytes(1, &cmd);

    // handleCmd(cmd);

    _curPos = pNext;
}
