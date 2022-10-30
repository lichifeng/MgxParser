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

void DefaultAnalyzer::_messagesAnalyzer(int debugFlag)
{
    _debugFlag = debugFlag;
    
    _curPos = _messagesStartPos;

    _skip(20);
    if (!IS_AOK(versionCode))
        _skip(4);

    _readPascalString(instructions);
    _guessEncoding();
    if (outEncoding != rawEncoding) {
        fixEncoding(instructions);
    }

    /*
    int totalStrs = IS_AOK(versionCode) ? 8 : 9; /// \todo 需要验证 AOK 的情况
    for (size_t i = 0; i < totalStrs; i++)
        _skipPascalString();

    /// \note 这一节的数据剩下的就不读了，没什么用。以后再说吧。
    */
}