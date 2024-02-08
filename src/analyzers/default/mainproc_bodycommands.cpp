/***************************************************************
 * \file       mainproc_bodycommands.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"
#include "auxiliary.h"

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
    status_.body_scanned_ = true;
}

void DefaultAnalyzer::ReadGameStart(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    if (cursor_(body_start_).Peek<uint32_t>() != 500)
        cursor_ >> 4; // Log version was read before
    // \note 其实规律是从这里开始读过24个字节后一直要往后走，直到遇到第一个02 00 00 00
    cursor_ >> sync_checksum_interval_
            >> is_multiplayer_
            >> 16; // pov & revealMap & containsSequenceNumbers & numberOfChapters, duplicated
    if (cursor_.Remain() >= 4 && cursor_.Peek<uint32_t>() == 0)
        cursor_ >> 4;
    if (cursor_.Remain() >= 8 && cursor_.Peek<uint32_t>() != 2)
        cursor_ >> 8;
}