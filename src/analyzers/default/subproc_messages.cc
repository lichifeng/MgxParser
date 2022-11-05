/**
 * \file       subProcessMessage.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-09-30
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

void DefaultAnalyzer::AnalyzeMessages(int debug_flag) {
    status_.debug_flag_ = debug_flag;

    cursor_(message_start_)
            >> (IS_AOK(version_code_) ? 20 : 24)
            >> instructions;

    DetectEncoding();

    cursor_.FixEncoding(instructions);
}