/***************************************************************
 * \file       addon_guesswinner.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <map>
#include <array>
#include <string>
#include "analyzer.h"

struct TeamCredit {
    uint32_t credits;
    uint32_t count;
    uint32_t avg;
};

void DefaultAnalyzer::JudgeWinner(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    std::multimap<uint8_t, TeamCredit, std::greater<uint8_t>> winner_credits;
    uint32_t index_max[9] = {0};
    uint8_t index_team[9] = {0};
    TeamCredit tc;
    uint32_t credit = 0, credit_max = 0;
    std::map<uint8_t, TeamCredit>::iterator found;
    bool all_survived = true; // If nobody had resigned, means POV quit first, then POV is possibly lost.

    for (auto &p: players) {
        if (!p.Valid())
            continue;

        // 如果resigned == -1，那存活时间就算成duration, 如果resigned >=0，那存活时间就是resigned，需要考虑的是双控的情况。
        if (-1 == p.resigned_) {
            credit = duration_ + 500; // \note 有时候投降时间会和时长一致，那就把没投降的人分数多给一点点，这样就不会判断为已经投降的和没投降都是赢。
        } else {
            credit = p.resigned_;
            all_survived = false;
        }
        p.resolved_teamid_ = 1 == p.resolved_teamid_ ? 10 + p.index : p.resolved_teamid_;
        index_team[p.index] = p.resolved_teamid_;
        if (credit > index_max[p.index])
            index_max[p.index] = credit;
    }

    for (int i = 0; i < 9; i++) {
        if (0 == index_team[i])
            continue;

        found = winner_credits.find(index_team[i]);
        if (found == winner_credits.end()) {
            tc.credits = index_max[i];
            tc.count = 1;
            winner_credits.emplace(std::make_pair(index_team[i], tc));
        } else {
            found->second.credits += index_max[i];
            found->second.count++;
        }
    }

    for (auto &t: winner_credits) {
        if (team_mode_.empty())
            team_mode_.append(std::to_string(t.second.count));
        else
            team_mode_.append("v").append(std::to_string(t.second.count));

        t.second.avg = t.second.credits / t.second.count;
        if (t.second.avg > credit_max)
            credit_max = t.second.avg;
    }

    for (auto &p: players) {
        if (!p.Valid())
            continue;

        found = winner_credits.find(p.resolved_teamid_);
        if (found != winner_credits.end() && found->second.avg == credit_max && !(p.slot == rec_player_ && all_survived))
            p.is_winner_ = true;
    }
}