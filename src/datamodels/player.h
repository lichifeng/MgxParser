/***************************************************************
 * \file       player.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#ifndef MGXPARSER_DATAMODEL_PLAYER_H_
#define MGXPARSER_DATAMODEL_PLAYER_H_

#include <cstdint>
#include <string>
#include "record.h"

class Player {
public:
    [[nodiscard]] bool Valid() const {
        return type_ >= 2 && type_ <= 5;
    } ///< \todo how about spectator?? Find a test record with spectators.

    [[nodiscard]] bool InitialDataFound() const { return data_offset_ > 0; }

    uint32_t dd_dlc_id_;
    uint32_t dd_color_id_ = UINT32_INIT;
    uint8_t de_selected_color_;
    uint8_t de_selected_teamid_ = 255;
    uint8_t resolved_teamid_ = 255; ///< Use this as final team. selected == 0x06 is random team but resolved 0x02-0x05 here
    uint32_t hd_data_crc_;
    std::string de_data_crc_;
    uint8_t dd_mp_gameversion_;
    // uint32_t            HD_teamIndex = 999; ///< This looks not right, use data from lobby
    uint32_t dd_civ_id_ = UINT32_INIT;
    std::string dd_ai_type_; ///< in 26.21: "PromiDE";
    uint8_t dd_ai_civname_index_;
    std::string dd_ai_name_;
    std::string name;
    uint32_t type_ = 999; ///< 0~6: absent, closed, human, eliminated, computer, cyborg, spectator
    uint64_t hd_steam_id_ = 0; ///< https://developer.valvesoftware.com/wiki/Steam_Web_API
    uint32_t de_profile_id_ = 0;
    int32_t dd_number_ = -1; ///< \todo 这个和index/slot怎么区别呢？
    uint32_t dd_rm_rating_ = 0;
    uint32_t dd_dm_rating_ = 0;
    uint8_t de_prefer_random_;
    uint8_t de_custom_ai_;
    uint32_t handicapping_level_ = UINT32_INIT;
    float modversion_raw_;
    float init_camera_[2] = {-1.0, -1.0};
    uint8_t civ_id_ = UINT8_INIT;
    uint8_t color_id_ = UINT8_INIT;

    // int32_t             startingAge = -1; ///< 各プレイヤーのゲーム開始する時代 (-1=設定なし, 0=暗黒, 1=領主, 2=城主, 3=帝王, 4=帝王の時代以降)
    int32_t index = -1;
    int32_t slot = -1;             ///< This number is not originally stored in a record. A slot means a seat in a game. Slots can operate a same index.
    std::vector<uint8_t> searchpattern_; ///< Used to locate player data in startinfo section
    uint32_t data_offset_ = 0;       ///< Offset from header stream start of player data in startinfo section. Keep this default 0.

    int32_t resigned_ = -1;
    bool disconnected_ = false;

    int32_t feudal_time_ = -1;
    int32_t castle_time_ = -1;
    int32_t imperial_time_ = -1;

    bool is_winner_ = false;
    bool updatedByPostgameBlock = false;
};

#endif //MGXPARSER_DATAMODEL_PLAYER_H_