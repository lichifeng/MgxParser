/***************************************************************
 * \file       subproc_detectversion.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::DetectVersion() {
    if (version_code_ != AOK)
        cursor_(body_start_) >> log_version_;
    cursor_(header_start_) >> version_string_ >> save_version_;
    // From DE version 75350
    if (save_version_ == -1) {
        uint32_t new_saveversion;
        cursor_ >> new_saveversion;
        save_version_ = (float)new_saveversion;
    }
    version_end_ = ai_start_ = cursor_();

    /// \todo Every condition needs to be tested!
    if (0 == strcmp(version_string_, "TRL 9.3")) {
        version_code_ = version_code_ == AOK ? AOKTRIAL : AOCTRIAL;
    } else if (0 == strcmp(version_string_, "VER 9.3")) {
        version_code_ = AOK;
    } else if (0 == strcmp(version_string_, "VER 9.4")) {
        if (log_version_ == 3 || log_version_ == 0) {
            // \warn log_version_ is sometimes 0 in aoc10 && VER 9.4, not fully sure
            version_code_ = AOC10;
        } else if (log_version_ == 5 || save_version_ >= 12.9699) {
            version_code_ = DE;
        } else if (save_version_ >= 12.4999) {
            version_code_ = HD50_6;
        } else if (save_version_ >= 12.4899) {
            version_code_ = HD48;
        } else if (save_version_ >= 12.3599) {
            version_code_ = HD46_7;
        } else if (save_version_ >= 12.3399) {
            version_code_ = HD43;
        } else if (save_version_ > 11.7601) {
            version_code_ = HD;
        } else if (log_version_ == 4) {
            version_code_ = AOC10C;
        } else {
            version_code_ = AOC;
        }
    } else if (0 == strcmp(version_string_, "VER 9.5")) {
        version_code_ = AOFE21;
    } else if (0 == strcmp(version_string_, "VER 9.8")) {
        version_code_ = USERPATCH12;
    } else if (0 == strcmp(version_string_, "VER 9.9")) {
        version_code_ = USERPATCH13;
    } else if (0 == strcmp(version_string_, "VER 9.A")) {
        version_code_ = USERPATCH14RC1;
    } else if (0 == strcmp(version_string_, "VER 9.B")) {
        version_code_ = USERPATCH14RC2;
    } else if (0 == strcmp(version_string_, "VER 9.C") || 0 == strcmp(version_string_, "VER 9.D")) {
        version_code_ = USERPATCH14;
    } else if (0 == strcmp(version_string_, "VER 9.E") || 0 == strcmp(version_string_, "VER 9.F")) {
        version_code_ = USERPATCH15;
    } else if (0 == strcmp(version_string_, "MCP 9.F")) {
        version_code_ = MCP;
    }

    if (UNDEFINED == version_code_) {
        throw std::string("Detected unsupported game version: ").append(version_string_);
    } else {
        status_.version_detected_ = true;
    }
}