/**
 * \file       subProcBody.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"
#include "bodyProcessors/helpers.h"

void DefaultAnalyzer::_readBodyCommands(int debugFlag)
{
    _debugFlag = debugFlag;
    
    int32_t opType;

    _readGameStart(21);

    _earlyMoveCnt = 0; // Reset earlymove recorder
    while (_remainBytes() >= 4)
    {
        _readBytes(4, &opType);
        switch (opType)
        {
        case OP_CHAT:
            _handleOpChat();
            break;
        case OP_SYNC:
            _handleOpSync();
            break;
        case OP_VIEWLOCK:
            _handleOpViewlock();
            break;
        case OP_COMMAND:
            _handleOpCommand();
            break;
        default:
            break;
        }
    }
}

void DefaultAnalyzer::_readGameStart(int debugFlag)
{
    _debugFlag = debugFlag;
    
    if (*(uint32_t *)_curPos != 500)
        _skip(4);
    /// \note 其实规律是从这里开始读过24个字节后一直要往后走，直到遇到第一个02
    /// 00 00 00
    _readBytes(4, &syncChecksumInterval);
    _readBytes(4, &isMultiplayer);
    _skip(16); // pov & revealMap & containsSequenceNumbers & numberOfChapters, duplicated
    //_readBytes(4, &_bodyVar.containsSequenceNumbers);
    //_readBytes(4, &_bodyVar.numberOfChapters);
    if (*(uint32_t *)_curPos == 0)
        _skip(4);
    if (*(uint32_t *)_curPos != 2)
        _skip(8);
}