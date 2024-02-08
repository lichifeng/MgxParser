/***************************************************************
 * \file       subproc_detectencoding.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"

/**
 * \todo aoc-mgz 中有更多关于编码和语言的关键字映射关系，可以套用过来。目前还有不少录像不能正确识别编码，比如 test/testRecords/AOC10c_2v2_4_717cd3fc.zip
 * \warning 不要随便更改这里的编码字符串，连大小写也不要改，其它地方的代码可能用于比对。
 */
void DefaultAnalyzer::DetectEncoding() {
    status_.encoding_detected_ = true;
    if (FindEncodingPattern(patterns::kZh)) {
        raw_encoding_ = "cp936";
    } else if (FindEncodingPattern(patterns::kZhUtf8)) {
        raw_encoding_ = "utf-8";
    } else if (FindEncodingPattern(patterns::kZhWide)) {
        raw_encoding_ = "cp936";
    } else if (FindEncodingPattern(patterns::kZhTw)) {
        raw_encoding_ = "cp950";
    } else if (FindEncodingPattern(patterns::kBr)) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::kDe)) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::kEn)) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::kEs)) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::kFr)) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::kIt)) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::kJp)) {
        raw_encoding_ = "cp932";
    } else if (FindEncodingPattern(patterns::kJpUtf8)) {
        raw_encoding_ = "utf-8";
    } else if (FindEncodingPattern(patterns::kKo)) {
        raw_encoding_ = "cp949";
    } else if (FindEncodingPattern(patterns::kKoUtf8)) {
        raw_encoding_ = "utf-8";
    } else if (FindEncodingPattern(patterns::kRu)) {
        raw_encoding_ = "windows-1251";
    } else if (FindEncodingPattern(patterns::kRuUtf8)) {
        raw_encoding_ = "windows-1251";
    } else if (FindEncodingPattern(patterns::kNl)) {
        raw_encoding_ = "windows-1252";
    } else if (IS_HD(version_code_) || IS_DE(version_code_)) {
        raw_encoding_ = "utf-8"; // \todo embeded map name is not extracted here
    } else {
        status_.encoding_detected_ = false;
    }

    if (!raw_encoding_.empty() && !out_encoding_.empty())
        cursor_.SetEncoding(raw_encoding_, out_encoding_);
}