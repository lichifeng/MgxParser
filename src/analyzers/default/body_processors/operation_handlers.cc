/***************************************************************
 * \file       operation_handlers.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <string>
#include "../analyzer.h"
#include "nlohmann_json_3/json.hpp"

using json = nlohmann::json;

uint32_t sync_data[2];
Chat tmp_chat;
uint32_t cmdlen;
int tmp_index;
std::size_t nextcmd;

/**
 * https://github.com/stefan-kolb/aoc-mgx-format/blob/master/spec/body/synchronization/Synchronization.md
 */
void DefaultAnalyzer::HandleSync() {
    cursor_ >> sync_data;
    duration_ += sync_data[0];
    cursor_ >> (0 == sync_data[1] ? 28 : 0) >> 12;
}

/**
 * "Viewpoints (for lock view) are only saved for the owner of the recorded game, together with the sychronization Info."
 */
void DefaultAnalyzer::HandleViewlock() {
    cursor_ >> 12; // 看起来是视角切换的命令，比如按H切到TC这种(4byte float, 4byte float, 4byte int)
}

/**
 * https://github.com/stefan-kolb/aoc-mgx-format/blob/master/spec/body/chat/0xFFFFFFFF%20(Chat).md
 */
void DefaultAnalyzer::HandleChat() {
    if (cursor_.Peek<int32_t>() != -1) {
        return;
    } else {
        cursor_ >> 4;
    }

    cursor_ >> tmp_chat.msg;
    tmp_index = tmp_chat.msg[2] - '0'; // Convert char to int
    // Two types of messages: "@#8... ..." in earlier versions and json strings in DE version
    if ('@' == tmp_chat.msg[0] && '#' == tmp_chat.msg[1]) {
        // Filter junk info like being attacked by animals, etc.
        if (tmp_index > 0 && tmp_index < 9
            && 0 != tmp_chat.msg.compare(3, players[tmp_index].name.size(), players[tmp_index].name)
            || players[tmp_index].name.empty()
            || tmp_index == 0)
            return;
    } else {
        try {
            auto j = json::parse(tmp_chat.msg);
            tmp_chat.msg = j.at("messageAGP");
        }
        catch (...) {
            tmp_chat.msg = "<json error>";
        }
    }
    if (!tmp_chat.msg.empty()) {
        tmp_chat.time = duration_;
        chat.emplace_back(tmp_chat);
    }
}

/**
 * https://github.com/stefan-kolb/aoc-mgx-format/tree/master/spec/body/actions
 */
void DefaultAnalyzer::HandleCommand() {
    // \todo Merge de71094 update from https://github.com/happyleavesaoc/aoc-mgz/commit/19b75b3b7e51e254075644ea2be96fdb0a7477b1 
    cursor_ >> cmdlen;
    nextcmd = cursor_.Remain() < cmdlen ? (cursor_() + cursor_.Remain()) : (cursor_() + cmdlen);

    HandleAction(cmdlen);

    cursor_(nextcmd);
}
