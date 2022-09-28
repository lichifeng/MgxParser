/**
 * \file       Player.h
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      Represents a player
 * \version    0.1
 * \date       2022-09-23
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */
#pragma once

#include <cstdint>
#include <string>

using namespace std;

class Player
{
public:
    bool                valid() { return type >= 2 && type <= 10; }

    uint32_t            DD_DLCID;
    uint32_t            DD_colorID;
    uint8_t             DE_selectedColor;
    uint8_t             DE_selectedTeamID;
    uint8_t             DE_resolvedTeamID;
    uint32_t            HD_datCrc;
    string              DE_datCrc;
    uint8_t             DD_MPGameVersion;
    uint32_t            HD_teamIndex;
    uint32_t            DD_civID;
    string              DD_AIType; ///< in 26.21: "PromiDE";
    uint8_t             DD_AICivNameIndex;
    string              DD_AIName;
    string              name = "";
    uint32_t            type = 999; ///< 0~6: absent, closed, human, eliminated, computer, cyborg, spectator
    uint64_t            HD_steamID;
    uint32_t            DE_profileID;
    int32_t             DD_playerNumber;
    uint32_t            DD_RMRating = 0;
    uint32_t            DD_DMRating = 0;
    uint8_t             DE_preferRandom;
    uint8_t             DE_customAI;
    char                DE_handicap[8];

    // int32_t             startingAge = -1; ///< 各プレイヤーのゲーム開始する時代 (-1=設定なし, 0=暗黒, 1=領主, 2=城主, 3=帝王, 4=帝王の時代以降)
    int32_t             slot = -1;
    vector<uint8_t>     searchPattern; ///< Used to locate player data in startinfo section
    uint32_t            dataOffset = 0; ///< Offset from header stream start of player data in startinfo section
};