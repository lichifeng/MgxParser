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
    if (_bytecmp(versionStr, "TRL 9.3", 8))
    {
        return versionCode == AOK ? AOKTRIAL : AOCTRIAL;
    }
    if (_bytecmp(versionStr, "VER 9.3", 8))
        return versionCode = AOK;
    if (_bytecmp(versionStr, "VER 9.4", 8))
    {
        if (logVersion == 3)
            return versionCode = AOC10;
        if (logVersion == 5 || saveVersion >= 12.9699)
            return versionCode = DE;
        if (saveVersion >= 12.4999)
            return versionCode = HD50_6;
        if (saveVersion >= 12.4899)
            return versionCode = HD48;
        if (saveVersion >= 12.3599)
            return versionCode = HD46_7;
        if (saveVersion >= 12.3399)
            return versionCode = HD43;
        if (saveVersion > 11.7601)
            return versionCode = HD;
        if (logVersion == 4)
            return versionCode = AOC10C;
        return versionCode = AOC;
    }
    if (_bytecmp(versionStr, "VER 9.5", 8))
        return versionCode = AOFE21;
    if (_bytecmp(versionStr, "VER 9.8", 8))
        return versionCode = USERPATCH12;
    if (_bytecmp(versionStr, "VER 9.9", 8))
        return versionCode = USERPATCH13;
    if (_bytecmp(versionStr, "VER 9.A", 8))
        return versionCode = USERPATCH14RC1;
    if (_bytecmp(versionStr, "VER 9.B", 8))
        return versionCode = USERPATCH14RC2;
    if (_bytecmp(versionStr, "VER 9.C", 8) || _bytecmp(versionStr, "VER 9.D", 8))
        return versionCode = USERPATCH14;
    if (_bytecmp(versionStr, "VER 9.E", 8) || _bytecmp(versionStr, "VER 9.F", 8))
        return versionCode = USERPATCH15;
    if (_bytecmp(versionStr, "MCP 9.F", 8))
        return versionCode = MCP;

    // If none above match:
    logger_->warn(
        "{}(): Detected unsupported game version: {}. @{}.",
        __FUNCTION__, versionStr, _distance());
    _sendExceptionSignal();
    
    return versionCode = UNSUPPORTED;
}