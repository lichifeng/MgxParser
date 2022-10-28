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

bool DefaultAnalyzer::_findEncodingPattern(const char *pattern, std::string &mapName, size_t patternLen)
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
 * \todo aoc-mgz 中有更多关于编码和语言的关键字映射关系，可以套用过来。目前还有不少录像不能正确识别编码，比如 test/testRecords/AOC10c_2v2_4_717cd3fc.zip
 * \warning 不要随便更改这里的编码字符串，连大小写也不要改，其它地方的代码可能用于比对。
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
        //rawEncoding = "cp936"; // \note 兼容性更好，有时候中文玩家会用英文版。其它字符集都向下兼容英文，保持默认更好。
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

    if (_encodingConverter == nullptr)
        _encodingConverter = new EncodingConverter(outEncoding, rawEncoding);
}