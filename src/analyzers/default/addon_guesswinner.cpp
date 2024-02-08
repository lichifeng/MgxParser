/***************************************************************
 * \file       addon_guesswinner.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <vector>
#include <map>
#include <string>
#include <algorithm>

#include "analyzer.h"

// 代表每个队伍的积分情况
struct TeamCredit {
    uint32_t credits = 0;
    uint32_t count = 1;
    uint32_t avg = 0;
};

// 存放每个index的积分和队伍信息
struct IndexInfo {
    uint32_t credit = 0;
    uint8_t team = 0;
};

void DefaultAnalyzer::JudgeWinner(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    uint32_t credit_max = 0;
    bool all_survived = true;  // If nobody had resigned, means POV quit first, then POV is possibly lost.

    // 根据存活情况，给每个人一个分数（credit）
    // 这个分数等于存活的时长，存活的越久，分数越高
    std::map<int32_t, IndexInfo> index_info;
    uint8_t recplayer_team;
    for (auto &p : players) {
        if (!p.Valid())
            continue;

        IndexInfo ii;
        // 如果resigned == -1，那存活时间就算成duration
        // 如果resigned>=0，那那表这个人投降了，存活时间就是resigned
        if (-1 == p.resigned_) {
            ii.credit =
                duration_ + 500;  // 投降时间可能等于时长，需要给没投降的人多点分，以免判断为已经投降的和没投降都赢
        } else {
            ii.credit = p.resigned_;
        }
        p.resolved_teamid_ = 1 == p.resolved_teamid_ ? 10 + p.index : p.resolved_teamid_;
        ii.team = p.resolved_teamid_;
        if (p.slot == rec_player_)
            recplayer_team = p.resolved_teamid_;

        auto index_profile = index_info.find(p.index);
        if (index_profile == index_info.end()) {
            // 如果这个index的信息还不存在：
            index_info.emplace(std::make_pair(p.index, ii));
        } else {
            // 如果这个index的信息已经存在（说明是多控）：
            if (ii.credit > index_profile->second.credit) {
                // 在多控的情况下，只取最高分：
                index_profile->second.credit = ii.credit;
            }
        }
    }

    // 数据格式：队伍编号 => 队伍积分
    std::multimap<uint8_t, TeamCredit, std::greater<uint8_t>> winner_credits;
    for (auto const &[k, v] : index_info) {
        // 先找有没有这个队伍的积分数据：
        auto found = winner_credits.find(v.team);
        if (found == winner_credits.end()) {
            // 如果没有找到，就初始化一个新的：
            TeamCredit tc;
            tc.credits = v.credit;
            winner_credits.emplace(std::make_pair(v.team, tc));
        } else {
            // 如果队伍信息已经存在了，就更新数据：
            found->second.credits += v.credit;
            found->second.count++;
        }
    }

    std::vector<uint32_t> teamMembers; // 用来生成组队模式（1v1, 4v4, 1v7, etc.）
    teamMembers.reserve(winner_credits.size());
    for (auto& t : winner_credits) {
        teamMembers.emplace_back(t.second.count);

        // 算队伍里的人均积分：
        t.second.avg = t.second.credits / t.second.count;
        if (t.second.avg < duration_ + 500)
            all_survived = false;
        if (t.second.avg > credit_max) {
            credit_max = t.second.avg;
        }
    }

    // 生成组队字符串
    std::sort(teamMembers.begin(), teamMembers.end());
    for (auto& c : teamMembers) {
        if (team_mode_.empty())
            team_mode_.append(std::to_string(c));
        else
            team_mode_.append("v").append(std::to_string(c));
    }

    // 有一种偶尔会出现的情况，就是HD版中玩家组队都为0，无法区分组队
    // 这种情况就把投降的玩家设定为输，没投降就算赢
    if (team_mode_.size() < 3) team_mode_ = "-";

    for (auto &p : players) {
        if (!p.Valid())
            continue;

        if (team_mode_.size() < 3) {
            p.is_winner_ = -1 == p.resigned_ ? true : false;
            continue;
        }

        auto found = winner_credits.find(p.resolved_teamid_);
        if (found != winner_credits.end() && found->second.avg == credit_max &&
            !(p.resolved_teamid_ == recplayer_team && all_survived))
            p.is_winner_ = true;
    }
}