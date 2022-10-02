/**
 * \file       subProcAI.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-03
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include "Analyzer.h"
#include "utils.h"

void DefaultAnalyzer::_AIAnalyzer()
{
    _readBytes(4, &indcludeAI);
    if (!indcludeAI)
        return;

    _skip(2);
    uint16_t numAIStrings = *(uint16_t *)_curPos;
    _skip(6);
    for (uint32_t i = 0; i < numAIStrings; i++)
    {
        _skip(4 + *(int32_t *)_curPos);
    }

    if (IS_DE(versionCode))
    {
        _skip(5);
    }
    else
    {
        _skip(4); /// \todo 验证一下各版本的情况
    }
    if (!_expectBytes(patterns::AIdataUnknown))
    {
        logger->warn(
            "{}(): Validation in AI failed, expecting [08 00] @{}.",
            __FUNCTION__, _distance());
        _failedSignal = true;
        return;
    }

    // AI Data
    if (IS_DE(versionCode))
    { /// \todo this takes too long, not acceptable
        auto curItr = _header.begin() + (_curPos - _curStream);
        uint16_t rulesCnt = 0;

        // // Tricky skip
        // for (size_t i = 0; i < _DD_AICount; i++)
        // {
        //     patterns::AIDirtyFix[4] = i;
        //     curItr = findPosition(
        //         curItr, _header.end(),
        //         patterns::AIDirtyFix.begin(),
        //         patterns::AIDirtyFix.end()
        //     );
        //     _curPos = &(*curItr);
        //     _skip(10);
        //     _readBytes(2, &rulesCnt);
        //     curItr += 200 * rulesCnt;
        //     cout << "rulesCnt: " << rulesCnt << endl;
        // }
        // _curPos = &(*curItr);

        // Dumb skip
        /// \todo 这里用了非常不确定的方法来跳过，没有充分验证。没有直接查找4096个0X00是出于性能考虑。
        patterns::FFs_500.resize(1000, 0x00);
        curItr = findPosition(
            curItr, _header.end(),
            patterns::FFs_500.begin(),
            patterns::FFs_500.end());
        _curPos = &(*curItr);
        _curPos += 4096 + 500;
    }
    else
    {
        int actionSize = 24;                 // See recanalyst
        int ruleSize = 16 + 16 * actionSize; // See recanalyst
        if (saveVersion > 11.9999)
            ruleSize += 0x180;

        for (uint16_t i = 0, numRules; i < 8; i++)
        {
            _skip(10);
            _readBytes(2, &numRules);
            if (numRules > 10000)
            {
                logger->warn("{}(): numRules in AI data > 10000 (maxRules is normally 10000) @{}.", __FUNCTION__, _distance());
                _failedSignal = true;
                return;
            }
            _skip(4);
            for (int j = 0; j++ < numRules; _skip(ruleSize))
                ;
        }

        _skip(1448); // 104 + 320 + 1024 \note 这里我在104个字节后发现了2624个FF，也不知道为什么，应该是1344
        if (saveVersion >= 11.9599)
            _skip(1280); /// \todo 针对这个版本号边界要验证一下
        _skip(4096);     // 4096个00
    }

    if (saveVersion >= 12.2999 && IS_HD(versionCode))
    {
        _skip(4); /// \todo 这里应该是 recanalyst 里的，需要验证。考虑到不确定性，最好加一段异常后尝试查找地图坐标位置的代码。
    }
}