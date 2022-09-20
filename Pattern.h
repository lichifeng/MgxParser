#pragma once

#include "Record.h"
#include <vector>

namespace pattern
{
    std::vector<BYTE> gameSettingEnd = { 0xe0, 0xab, 0x45, 0x9a, 0x99, 0x99, 0x99, 0x99, 0x99, 0xf9, 0x3f };
    std::vector<BYTE> separator = { 0x9d, 0xff, 0xff, 0xff };
    std::vector<BYTE> scenarioConstant = { 0xf6, 0x28, 0x9c, 0x3f };
    std::vector<BYTE> aokSeparator = { 0x9a, 0x99, 0x99, 0x3f };
    std::vector<BYTE> aoe2recordScenarioSeparator = { 0xae, 0x47, 0xa1, 0x3f };
    std::vector<BYTE> aoe2recordHeaderSeparator = { 0xa3, 0x5f, 0x02, 0x00 };
    std::vector<BYTE> startInfoTrailSeparator = { 0x16, 0xc6, 0x00, 0x00, 0x00 };
    std::vector<BYTE> existObjectSeparator = { 0x0B, 0x00, 0x08, 0x00, 0x00, 0x00, 0x02, 0x00, 0x00 };
    std::vector<BYTE> objectEndSeparator = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x80, 0xBF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
        0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00 };
    std::vector<BYTE> aokObjectEndSeparator = { 0xFF, 0xFF, 0xFF, 0xFF, 0x00, 0x00, 0x80, 0xBF,
        0x00, 0x00, 0x80, 0xBF, 0x00, 0x00, 0x00, 0x00, 0x00 };
    std::vector<BYTE> playerInfoEndSeparator = { 0x00, 0x0B, 0x00, 0x02, 0x00, 0x00,
        0x00, 0x02, 0x00, 0x00, 0x00, 0x0B };
    std::vector<BYTE> objectsMidSeparatorGaia = { 0x00, 0x0B, 0x00, 0x40, 0x00, 0x00, 0x00, 0x20, 0x00, 0x00 };
    std::vector<BYTE> aoe2headerSeparator = { 0xa3,0x5f, 0x02, 0x00 };

    // TODO SOME PATTERN WAS NOT VALIDATED!
    constexpr char zh_pattern[] = { 
        static_cast<char>(0xb5), 
        static_cast<char>(0xd8), 
        static_cast<char>(0xcd), 
        static_cast<char>(0xbc), 
        static_cast<char>(0xc0), 
        static_cast<char>(0xe0), 
        static_cast<char>(0xb1), 
        static_cast<char>(0xf0), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
    constexpr char zh_utf8_pattern[] = {
        static_cast<char>(0xe5),
        static_cast<char>(0x9c),
        static_cast<char>(0xb0),
        static_cast<char>(0xe5),
        static_cast<char>(0x9b),
        static_cast<char>(0xbe),
        static_cast<char>(0xe7),
        static_cast<char>(0xb1),
        static_cast<char>(0xbb),
        static_cast<char>(0xe5),
        static_cast<char>(0x9e),
        static_cast<char>(0x8b),
        static_cast<char>(0x3a),
        static_cast<char>(0x20)
        };
    constexpr char zh_wide_pattern[] = { 
        static_cast<char>(0xb5), 
        static_cast<char>(0xd8), 
        static_cast<char>(0xcd), 
        static_cast<char>(0xbc), 
        static_cast<char>(0xc0), 
        static_cast<char>(0xe0), 
        static_cast<char>(0xb1), 
        static_cast<char>(0xf0), 
        static_cast<char>(0xa3), 
        static_cast<char>(0xba)
        };
    constexpr char zh_tw_pattern[] = { 
        static_cast<char>(0xa6), 
        static_cast<char>(0x61), 
        static_cast<char>(0xb9), 
        static_cast<char>(0xcf), 
        static_cast<char>(0xc3), 
        static_cast<char>(0xfe), 
        static_cast<char>(0xa7), 
        static_cast<char>(0x4f), 
        static_cast<char>(0xa1), 
        static_cast<char>(0x47)
        };
    constexpr char br_pattern[] = "Tipo de Mapa: ";
    constexpr char de_pattern[] = "Kartentyp: ";
    constexpr char en_pattern[] = "Map Type: ";
    constexpr char es_pattern[] = "Tipo de mapa: ";
    constexpr char fr_pattern[] = "Type de carte : ";
    constexpr char it_pattern[] = "Tipo di mappa: ";
    constexpr char jp_pattern[] = { 
        static_cast<char>(0x83), 
        static_cast<char>(0x7d), 
        static_cast<char>(0x83), 
        static_cast<char>(0x62), 
        static_cast<char>(0x83), 
        static_cast<char>(0x76), 
        static_cast<char>(0x82), 
        static_cast<char>(0xcc), 
        static_cast<char>(0x8e), 
        static_cast<char>(0xed), 
        static_cast<char>(0x97), 
        static_cast<char>(0xde), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
    constexpr char jp_utf8_pattern[] = { 
        static_cast<char>(0xe3), 
        static_cast<char>(0x83), 
        static_cast<char>(0x9e), 
        static_cast<char>(0xe3), 
        static_cast<char>(0x83), 
        static_cast<char>(0x83), 
        static_cast<char>(0xe3), 
        static_cast<char>(0x83), 
        static_cast<char>(0x97), 
        static_cast<char>(0xe3), 
        static_cast<char>(0x81), 
        static_cast<char>(0xae), 
        static_cast<char>(0xe7), 
        static_cast<char>(0xa8), 
        static_cast<char>(0xae), 
        static_cast<char>(0xe9), 
        static_cast<char>(0xa1), 
        static_cast<char>(0x9e), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
    constexpr char ko_pattern[] = { 
        static_cast<char>(0xc1), 
        static_cast<char>(0xf6), 
        static_cast<char>(0xb5), 
        static_cast<char>(0xb5), 
        static_cast<char>(0x20), 
        static_cast<char>(0xc1), 
        static_cast<char>(0xbe), 
        static_cast<char>(0xb7), 
        static_cast<char>(0xf9), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
    constexpr char ko_utf8_pattern[] = { 
        static_cast<char>(0xec), 
        static_cast<char>(0xa7), 
        static_cast<char>(0x80), 
        static_cast<char>(0xeb), 
        static_cast<char>(0x8f), 
        static_cast<char>(0x84), 
        static_cast<char>(0x20), 
        static_cast<char>(0xec), 
        static_cast<char>(0xa2), 
        static_cast<char>(0x85), 
        static_cast<char>(0xeb), 
        static_cast<char>(0xa5), 
        static_cast<char>(0x98), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
    constexpr char nl_pattern[] = "Kaarttype: ";
    constexpr char ru_pattern[] = { 
        static_cast<char>(0xd2), 
        static_cast<char>(0xe8), 
        static_cast<char>(0xef), 
        static_cast<char>(0x20), 
        static_cast<char>(0xea), 
        static_cast<char>(0xe0), 
        static_cast<char>(0xf0), 
        static_cast<char>(0xf2), 
        static_cast<char>(0xfb), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
    constexpr char ru_utf8_pattern[] = { 
        static_cast<char>(0xd0), 
        static_cast<char>(0xa2), 
        static_cast<char>(0xd0), 
        static_cast<char>(0xb8), 
        static_cast<char>(0xd0), 
        static_cast<char>(0xbf), 
        static_cast<char>(0x20), 
        static_cast<char>(0xd0), 
        static_cast<char>(0xba), 
        static_cast<char>(0xd0), 
        static_cast<char>(0xb0), 
        static_cast<char>(0xd1), 
        static_cast<char>(0x80), 
        static_cast<char>(0xd1), 
        static_cast<char>(0x82), 
        static_cast<char>(0xd1), 
        static_cast<char>(0x8b), 
        static_cast<char>(0x3a), 
        static_cast<char>(0x20)
        };
}