/**
 * \file       cmdHandler.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-22
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */
#include <cstdint>
#include "../Analyzer.h"

#include <iostream>

void DefaultAnalyzer::_handleAction()
{
    uint8_t rD[3];
    int8_t playerSlot;
    int16_t techID;

    uint8_t cmd = _curPos[0];

    switch (cmd)
    {
    case COMMAND_RESIGN:
        // int8 :action_identifier
        // int8 :player_number
        // int8 :player_id
        // int32 :disconnect

        _readBytes(3, rD);
        if (players[rD[1]].valid())
        {
            players[rD[1]].resigned = duration;
            if (!players[rD[1]].disconnected)
                players[rD[1]].disconnected = (bool)*(int32_t *)_curPos;
        }
        // logger->info("Resigned number:{}, index:{}", rD[1], rD[2]);
        break;

    case COMMAND_RESEARCH:
        // int8 :action_identifier
        // int24 :zero
        // int32 :building_id
        // int8 :player_number
        // int8 :zero2
        // int16 :technology_id
        // int32 :const
        _skip(8);
        _readBytes(1, &playerSlot);
        if (playerSlot < 0 || playerSlot > 8 || !players[playerSlot].valid())
            break;
        _skip(1);
        _readBytes(2, &techID);

        if (RESEARCH_FEUDAL == (int)techID)
        {
            players[playerSlot].feudalTime = duration + 0.5 + 130000;
        }
        else if (RESEARCH_CASTLE == (int)techID)
        {
            players[playerSlot].castleTime = duration + 0.5 \
            + (8 == players[playerSlot].civID ? 160000 : 160000 / 1.10);
        }
        else if (RESEARCH_IMPERIAL == (int)techID)
        {
            players[playerSlot].imperialTime = duration + 0.5 \
            + (8 == players[playerSlot].civID ? 190000 : 190000 / 1.10);
        }
        break;

    default:
        break;
    }
}