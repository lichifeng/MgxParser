/**
 * \file       subProcEncodingHandler.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-09-30
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "analyzer.h"

bool DefaultAnalyzer::FindEncodingPattern(const char *pattern, std::string &map_name, size_t pattern_len) {
    size_t pos, pos_end;

    if (string::npos != (pos = instructions.find(pattern))) {
        pos_end = instructions.find('\n', pos + pattern_len);
        if (string::npos != pos_end)
            embeded_mapname_ = instructions.substr(pos + pattern_len, pos_end - pos - pattern_len);

        return true;
    }

    return false;
}

/**
 * \todo aoc-mgz 中有更多关于编码和语言的关键字映射关系，可以套用过来。目前还有不少录像不能正确识别编码，比如 test/testRecords/AOC10c_2v2_4_717cd3fc.zip
 * \warning 不要随便更改这里的编码字符串，连大小写也不要改，其它地方的代码可能用于比对。
 */
void DefaultAnalyzer::DetectEncoding() {

    if (FindEncodingPattern(patterns::zh_pattern, embeded_mapname_, size(patterns::zh_pattern))) {
        raw_encoding_ = "cp936";
    } else if (FindEncodingPattern(patterns::zh_utf8_pattern, embeded_mapname_, size(patterns::zh_utf8_pattern))) {
        raw_encoding_ = "utf-8";
    } else if (FindEncodingPattern(patterns::zh_wide_pattern, embeded_mapname_, size(patterns::zh_wide_pattern))) {
        raw_encoding_ = "cp936";
    } else if (FindEncodingPattern(patterns::zh_tw_pattern, embeded_mapname_, size(patterns::zh_tw_pattern))) {
        raw_encoding_ = "cp950";
    } else if (FindEncodingPattern(patterns::br_pattern, embeded_mapname_, size(patterns::br_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::de_pattern, embeded_mapname_, size(patterns::de_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::en_pattern, embeded_mapname_, size(patterns::en_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::es_pattern, embeded_mapname_, size(patterns::es_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::fr_pattern, embeded_mapname_, size(patterns::fr_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::it_pattern, embeded_mapname_, size(patterns::it_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (FindEncodingPattern(patterns::jp_pattern, embeded_mapname_, size(patterns::jp_pattern))) {
        raw_encoding_ = "cp932";
    } else if (FindEncodingPattern(patterns::jp_utf8_pattern, embeded_mapname_, size(patterns::jp_utf8_pattern))) {
        raw_encoding_ = "utf-8";
    } else if (FindEncodingPattern(patterns::ko_pattern, embeded_mapname_, size(patterns::ko_pattern))) {
        raw_encoding_ = "cp949";
    } else if (FindEncodingPattern(patterns::ko_utf8_pattern, embeded_mapname_, size(patterns::ko_utf8_pattern))) {
        raw_encoding_ = "utf-8";
    } else if (FindEncodingPattern(patterns::ru_pattern, embeded_mapname_, size(patterns::ru_pattern))) {
        raw_encoding_ = "windows-1251";
    } else if (FindEncodingPattern(patterns::ru_utf8_pattern, embeded_mapname_, size(patterns::ru_utf8_pattern))) {
        raw_encoding_ = "windows-1251";
    } else if (FindEncodingPattern(patterns::nl_pattern, embeded_mapname_, size(patterns::nl_pattern))) {
        raw_encoding_ = "windows-1252";
    } else if (IS_HD(version_code_) || IS_DE(version_code_)) {
        raw_encoding_ = "utf-8";
    }

    if (!raw_encoding_.empty() && !out_encoding_.empty())
        cursor_.SetEncoding(raw_encoding_, out_encoding_);
}