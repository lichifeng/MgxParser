/**
 * \file       subProcVictory.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeVictory(int debug_flag) {
    status_.debug_flag_ = debug_flag;
    cursor_(victory_start_ = disabledtechs_start_ - 12544 - 44)
            >> 4
            >> victoryIsConquest
            >> 4
            >> victoryRelics
            >> 4
            >> victoryExplored
            >> 4
            >> victoryAnyOrAll
            >> victoryMode
            >> victoryScore
            >> victoryTime;
}