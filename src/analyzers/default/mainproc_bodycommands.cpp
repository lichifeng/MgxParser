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
#include "body_processors/auxiliary.h"

void DefaultAnalyzer::ReadBodyCommands(int debug_flag) {
    status_.debug_flag_ = debug_flag;
    earlymove_count_ = 0; // Reset earlymove recorder

    int32_t op_type;
    ReadGameStart(21);
    while (cursor_.Remain() >= 4) {
        cursor_ >> op_type;
        switch (op_type) {
            case OP_CHAT:
                HandleChat();
                break;
            case OP_SYNC:
                HandleSync();
                break;
            case OP_VIEWLOCK:
                HandleViewlock();
                break;
            case OP_COMMAND:
                HandleCommand();
                break;
            default:
                break;
        }
    }
}

void DefaultAnalyzer::ReadGameStart(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    if (cursor_(body_start_).Peek<uint32_t>() != 500)
        cursor_ >> 4; // Log version was read at the beginning
    // \note 其实规律是从这里开始读过24个字节后一直要往后走，直到遇到第一个02 00 00 00
    cursor_ >> syncChecksumInterval
            >> isMultiplayer
            >> 16; // pov & revealMap & containsSequenceNumbers & numberOfChapters, duplicated
    if (cursor_.Peek<uint32_t>() == 0)
        cursor_ >> 4;
    if (cursor_.Peek<uint32_t>() != 2)
        cursor_ >> 8;
}