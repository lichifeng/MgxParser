/**
 * \file       subProcVersionHandler.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-03
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

int DefaultAnalyzer::_setVersionCode()
{
    /// \todo Every condition needs to be tested!
    if (_bytecmp(version_string_, "TRL 9.3", 8))
    {
        return version_code_ == AOK ? AOKTRIAL : AOCTRIAL;
    }
    if (_bytecmp(version_string_, "VER 9.3", 8))
        return version_code_ = AOK;
    if (_bytecmp(version_string_, "VER 9.4", 8))
    {
        if (log_version_ == 3)
            return version_code_ = AOC10;
        if (log_version_ == 5 || save_version_ >= 12.9699)
            return version_code_ = DE;
        if (save_version_ >= 12.4999)
            return version_code_ = HD50_6;
        if (save_version_ >= 12.4899)
            return version_code_ = HD48;
        if (save_version_ >= 12.3599)
            return version_code_ = HD46_7;
        if (save_version_ >= 12.3399)
            return version_code_ = HD43;
        if (save_version_ > 11.7601)
            return version_code_ = HD;
        if (log_version_ == 4)
            return version_code_ = AOC10C;
        return version_code_ = AOC;
    }
    if (_bytecmp(version_string_, "VER 9.5", 8))
        return version_code_ = AOFE21;
    if (_bytecmp(version_string_, "VER 9.8", 8))
        return version_code_ = USERPATCH12;
    if (_bytecmp(version_string_, "VER 9.9", 8))
        return version_code_ = USERPATCH13;
    if (_bytecmp(version_string_, "VER 9.A", 8))
        return version_code_ = USERPATCH14RC1;
    if (_bytecmp(version_string_, "VER 9.B", 8))
        return version_code_ = USERPATCH14RC2;
    if (_bytecmp(version_string_, "VER 9.C", 8) || _bytecmp(version_string_, "VER 9.D", 8))
        return version_code_ = USERPATCH14;
    if (_bytecmp(version_string_, "VER 9.E", 8) || _bytecmp(version_string_, "VER 9.F", 8))
        return version_code_ = USERPATCH15;
    if (_bytecmp(version_string_, "MCP 9.F", 8))
        return version_code_ = MCP;

    // If none above match:
    logger_->warn(
            "{}(): Detected unsupported game version: {}. @{}.",
            __FUNCTION__, version_string_, _distance());
    _sendExceptionSignal();
    
    return version_code_ = UNSUPPORTED;
}