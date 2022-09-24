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

class Player
{
public:
    Player():
        AIType("test"), AIName("test1"), name("test2")
    {}
    uint32_t DLCID;
    uint32_t colorID;
    uint32_t datCrc;
    uint8_t MPGameVersion;
    uint32_t teamIndex;
    uint32_t civID;
    string AIType;
    uint8_t AICivNameIndex;
    string AIName;
    string name;
    uint32_t playerType; ///< 0~6: absent, closed, human, eliminated, computer, cyborg, spectator
    uint64_t steamID;
    int32_t playerNumber;
    uint32_t HDRMRating = 0;
    uint32_t HDDMRating = 0;
};