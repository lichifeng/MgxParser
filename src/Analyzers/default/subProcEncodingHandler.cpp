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

#include "Analyzer.h"

bool DefaultAnalyzer::_findEncodingPattern(const char* pattern, std::string& mapName, size_t patternLen)
{
    size_t pos, posEnd;

    if (string::npos != (pos = instructions.find(pattern)))
    {
        posEnd = instructions.find('\n', pos + patternLen);
        if (string::npos != posEnd)
            embededMapName = instructions.substr(pos + patternLen, posEnd - pos - patternLen);

        return true;
    }

    return false;
}

/**
 * \todo aoc-mgz 中有更多关于编码和语言的关键字映射关系，可以套用过来。
 * 
 */
void DefaultAnalyzer::_guessEncoding()
{

    if (_findEncodingPattern(patterns::zh_pattern, embededMapName, size(patterns::zh_pattern)))
    {
        rawEncoding = "cp936";
    }
    else if (_findEncodingPattern(patterns::zh_utf8_pattern, embededMapName, size(patterns::zh_utf8_pattern)))
    {
        rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(patterns::zh_wide_pattern, embededMapName, size(patterns::zh_wide_pattern)))
    {
        rawEncoding = "cp936";
    }
    else if (_findEncodingPattern(patterns::zh_tw_pattern, embededMapName, size(patterns::zh_tw_pattern)))
    {
        rawEncoding = "cp950";
    }
    else if (_findEncodingPattern(patterns::br_pattern, embededMapName, size(patterns::br_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::de_pattern, embededMapName, size(patterns::de_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::en_pattern, embededMapName, size(patterns::en_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::es_pattern, embededMapName, size(patterns::es_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::fr_pattern, embededMapName, size(patterns::fr_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::it_pattern, embededMapName, size(patterns::it_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(patterns::jp_pattern, embededMapName, size(patterns::jp_pattern)))
    {
        rawEncoding = "cp932";
    }
    else if (_findEncodingPattern(patterns::jp_utf8_pattern, embededMapName, size(patterns::jp_utf8_pattern)))
    {
        rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(patterns::ko_pattern, embededMapName, size(patterns::ko_pattern)))
    {
        rawEncoding = "cp949";
    }
    else if (_findEncodingPattern(patterns::ko_utf8_pattern, embededMapName, size(patterns::ko_utf8_pattern)))
    {
        rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(patterns::ru_pattern, embededMapName, size(patterns::ru_pattern)))
    {
        rawEncoding = "windows-1251";
    }
    else if (_findEncodingPattern(patterns::ru_utf8_pattern, embededMapName, size(patterns::ru_utf8_pattern)))
    {
        rawEncoding = "windows-1251";
    }
    else if (_findEncodingPattern(patterns::nl_pattern, embededMapName, size(patterns::nl_pattern)))
    {
        rawEncoding = "windows-1252";
    }
    else if (IS_HD(versionCode) || IS_DE(versionCode))
    {
        rawEncoding = "utf-8";
    }
    else if (rawEncoding.size() == 0)
    {
        rawEncoding = "gbk";
    }

    if (_encodingConverter == nullptr)
        _encodingConverter = new EncodingConverter(outEncoding, rawEncoding);
}