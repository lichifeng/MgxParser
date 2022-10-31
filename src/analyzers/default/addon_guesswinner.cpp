/**
 * \file       addonGuessWinner.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-22
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include <map>
#include <array>
#include <string>
#include "analyzer.h"

using namespace std;

struct TeamCredit
{
    uint32_t credits;
    uint32_t count;
    uint32_t avg;
};

/**
 * \brief      谁赢谁输只能靠猜。逻辑是：哪一组的人均存活时间最长，哪一组就算赢。
 * \details    如果resigned == -1，那存活时间就算成duration, 如果resigned >=0，那存活时间就是resigned，需要考虑的是双控的情况。
 *
 */
void DefaultAnalyzer::_guessWinner(int debugFlag)
{
    _debugFlag = debugFlag;

    multimap<uint8_t, TeamCredit> winnerCredits;
    uint32_t indexMax[9] = {0};
    uint8_t indexTeam[9] = {0};
    TeamCredit tc;
    uint32_t credit = 0, creditMax = 0;
    map<uint8_t, TeamCredit>::iterator found;
    bool allLived = true; // When nobody had resigned, that mean POV quit first, then POV is possibly lost.

    for (auto &p : players)
    {
        if (!p.valid())
            continue;

        if (p.index < 0 || p.index > 8)
        {
            logger_->warn("Flag:{}, bad player index:{}! Name: {}", _debugFlag, p.index, p.name);
        }
        if (-1 == p.resigned)
        {
            credit = duration + 500; // \note 有时候投降时间会和时长一致，那就把没投降的人分数多给一点点，这样就不会判断为已经投降的和没投降都是赢。
        }
        else
        {
            credit = p.resigned;
            allLived = false;
        }
        p.resolvedTeamID = 1 == p.resolvedTeamID ? 10 + p.index : p.resolvedTeamID;
        indexTeam[p.index] = p.resolvedTeamID;
        if (credit > indexMax[p.index])
            indexMax[p.index] = credit;
    }

    for (int i = 0; i < 9; i++)
    {
        if (0 == indexTeam[i])
            continue;

        found = winnerCredits.find(indexTeam[i]);
        if (found == winnerCredits.end())
        {
            tc.credits = indexMax[i];
            tc.count = 1;
            winnerCredits.emplace(make_pair(indexTeam[i], tc));
        }
        else
        {
            found->second.credits += indexMax[i];
            found->second.count++;
        }
    }

    for (auto &t : winnerCredits)
    {
        if (teamMode.empty())
            teamMode.append(to_string(t.second.count));
        else
            teamMode.append("v").append(to_string(t.second.count));

        t.second.avg = t.second.credits / t.second.count;
        if (t.second.avg > creditMax)
            creditMax = t.second.avg;
    }

    for (auto &p : players)
    {
        if (!p.valid())
            continue;

        found = winnerCredits.find(p.resolvedTeamID);
        if (found != winnerCredits.end() && found->second.avg == creditMax && !(p.slot == recPlayer && allLived))
            p.isWinner = true;
    }
}