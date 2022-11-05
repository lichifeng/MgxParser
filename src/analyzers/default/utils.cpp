/**
 * \file       utils.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-03
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include <vector>

namespace patterns {
    std::vector<uint8_t> ZEROs_4096(4096, 0x00);
    std::vector<uint8_t> FFs_500(500, 0xff);
    std::vector<uint8_t> gameSettingSign = {0x00, 0xe0, 0xab, 0x45};
    std::vector<uint8_t> separator = {0x9d, 0xff, 0xff, 0xff};
    std::vector<uint8_t> scenarioConstantAOC = {0xf6, 0x28, 0x9c, 0x3f};
    std::vector<uint8_t> scenarioConstantHD = {0xae, 0x47, 0xa1, 0x3f};
    std::vector<uint8_t> scenarioConstantAOK = {0x9a, 0x99, 0x99, 0x3f};
    std::vector<uint8_t> scenarioConstantMGX2 = {0xa4, 0x70, 0x9d, 0x3f};
}