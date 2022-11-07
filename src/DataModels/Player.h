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
#include "DataModels/DataModel.h"

using namespace std;

class Player
{
public:
    bool Valid() { return type_ >= 2 && type_ <= 5; } ///< \todo how about spectator?? Find a test record with spectators.
    bool InitialDataFound() { return data_offset_ > 0; }

    uint32_t dd_dlc_id_;
    uint32_t dd_color_id_ = UINT32_INIT;
    uint8_t de_selected_color_;
    uint8_t de_selected_teamid_ = 255;
    uint8_t resolved_teamid_ = 255; ///< Use this as final team. selected == 0x06 is random team but resolved 0x02-0x05 here
    uint32_t HD_datCrc;
    string de_data_crc_;
    uint8_t dd_mp_gameversion_;
    // uint32_t            HD_teamIndex = 999; ///< This looks not right, use data from lobby
    uint32_t dd_civ_id_ = UINT32_INIT;
    string dd_ai_type_; ///< in 26.21: "PromiDE";
    uint8_t DD_AICivNameIndex;
    string DD_AIName;
    string name = "";
    uint32_t type_ = 999; ///< 0~6: absent, closed, human, eliminated, computer, cyborg, spectator
    uint64_t HD_steamID = 0; ///< https://developer.valvesoftware.com/wiki/Steam_Web_API
    uint32_t DE_profileID = 0;
    int32_t DD_playerNumber = -1; ///< \todo 这个和index/slot怎么区别呢？
    uint32_t DD_RMRating = 0;
    uint32_t DD_DMRating = 0;
    uint8_t DE_preferRandom;
    uint8_t DE_customAI;
    uint32_t handicappingLevel = UINT32_INIT;
    float modversion_id_;
    float init_camera_[2] = {-1.0, -1.0};
    uint8_t civ_id_ = UINT8_INIT;
    uint8_t color_id_ = UINT8_INIT;

    // int32_t             startingAge = -1; ///< 各プレイヤーのゲーム開始する時代 (-1=設定なし, 0=暗黒, 1=領主, 2=城主, 3=帝王, 4=帝王の時代以降)
    int32_t index = -1;
    int32_t slot = -1;             ///< This number is not originally stored in a record. A slot means a seat in a game. Slots can operate a same index.
    vector<uint8_t> searchpattern_; ///< Used to locate player data in startinfo section
    uint32_t data_offset_ = 0;       ///< Offset from header stream start of player data in startinfo section. Keep this default 0.

    int32_t resigned_ = -1;
    bool disconnected_ = false;

    int32_t feudal_time_ = -1;
    int32_t castle_time_ = -1;
    int32_t imperial_time_ = -1;

    bool is_winner_ = false;
};