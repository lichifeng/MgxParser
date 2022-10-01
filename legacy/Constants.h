#pragma once
#include <map>

#define HEADER_INIT 4*1024*1024 ///< \todo moved to src/Analyzers/default/Analyzer.cpp, delete this
#define ZLIB_CHUNK 8192 ///< \todo moved to src/Analyzers/default/Analyzer.cpp, delete this
#define PLAYERNAME_MAX 16 // 15 chars permitted
#define PREMSG_MAX 248 // 247 chars input permitted
#define PREGAMECHAT_MAX 50

#define OP_COMMAND              0x01
#define OP_SYNC                 0x02
#define OP_VIEWLOCK             0x03
#define OP_CHAT                 0x04

#define COMMAND_RESIGN          0x0b
#define COMMAND_RESEARCH        0x65
#define COMMAND_TRAIN           0x77
#define COMMAND_TRAIN_SINGLE    0x64
#define COMMAND_BUILD           0x66
#define COMMAND_TRIBUTE         0x6c
#define COMMAND_POSTGAME        0xff
#define RESEARCH_FEUDAL         101
#define RESEARCH_CASTLE         102
#define RESEARCH_IMPERIAL       103

#define UT_EYECANDY             10
#define UT_FLAG                 20
#define UT_DEAD_FISH            30
#define UT_BIRD                 40
#define UT_UNKNOWN              50
#define UT_PROJECTILE           60
#define UT_CREATABLE            70
#define UT_BUILDING             80

#define TYPE_RANDOMMAP 0
#define TYPE_REGICIDE 1
#define TYPE_DEATHMATCH 2
#define TYPE_SCENARIO 3
#define TYPE_CAMPAIGN 4
#define TYPE_KINGOFTHEHILL 5
#define TYPE_WONDERRACE 6
#define TYPE_DEFENDTHEWONDER 7
#define TYPE_TURBORANDOMMAP 8

#define MAPSTYLE_STANDARD 0
#define MAPSTYLE_REALWORLD 1
#define MAPSTYLE_CUSTOM 2
#define MAPSTYLE_SPECIAL 3

#define LEVEL_HARDEST 0
#define LEVEL_HARD 1
#define LEVEL_MODERATE 2
#define LEVEL_STANDARD 3
#define LEVEL_EASIEST 4

#define SPEED_SLOW 100
#define SPEED_NORMAL 150
#define SPEED_FAST 200

#define REVEAL_NORMAL 0
#define REVEAL_EXPLORED 1
#define REVEAL_ALLVISIBLE 2

#define SIZE_TINY 0
#define SIZE_SMALL 1
#define SIZE_MEDIUM 2
#define SIZE_NORMAL 3
#define SIZE_LARGE 4
#define SIZE_GIANT 5

#define MODE_SINGLEPLAYER 0
#define MODE_MULTIPLAYER 1

namespace units
{
    // Unit IDs that we might draw on maps
    // GAIA (needed for colours when drawing)
    constexpr int GOLDMINE = 66;
    constexpr int STONEMINE = 102;
    constexpr int CLIFF1 = 264;
    constexpr int CLIFF2 = 265;
    constexpr int CLIFF3 = 266;
    constexpr int CLIFF4 = 267;
    constexpr int CLIFF5 = 268;
    constexpr int CLIFF6 = 269;
    constexpr int CLIFF7 = 270;
    constexpr int CLIFF8 = 271;
    constexpr int CLIFF9 = 272;
    constexpr int CLIFF10 = 273;
    constexpr int RELIC = 285;
    constexpr int TURKEY = 833;
    constexpr int SHEEP = 594;
    constexpr int DEER = 65;
    constexpr int BOAR = 48;
    constexpr int JAVELINA = 822;
    constexpr int FORAGEBUSH = 59;
    // Gates (needed for directions when drawing)
    constexpr int GATE = 487;
    constexpr int GATE2 = 490;
    constexpr int GATE3 = 665;
    constexpr int GATE4 = 673;
    constexpr int PALISADE_GATE = 792;
    constexpr int PALISADE_GATE2 = 796;
    constexpr int PALISADE_GATE3 = 800;
    constexpr int PALISADE_GATE4 = 804;
}

namespace maps
{
    constexpr int ARABIA = 9;
    constexpr int ARCHIPELAGO = 10;
    constexpr int BALTIC = 11;
    constexpr int BLACKFOREST = 12;
    constexpr int COASTAL = 13;
    constexpr int CONTINENTAL = 14;
    constexpr int CRATERLAKE = 15;
    constexpr int FORTRESS = 16;
    constexpr int GOLDRUSH = 17;
    constexpr int HIGHLAND = 18;
    constexpr int ISLANDS = 19;
    constexpr int MEDITERRANEAN = 20;
    constexpr int MIGRATION = 21;
    constexpr int RIVERS = 22;
    constexpr int TEAMISLANDS = 23;
    constexpr int RANDOM = 24;
    constexpr int SCANDINAVIA = 25;
    constexpr int MONGOLIA = 26;
    constexpr int YUCATAN = 27;
    constexpr int SALTMARSH = 28;
    constexpr int ARENA = 29;
    constexpr int KINGOFTHEHILL = 30;
    constexpr int OASIS = 31;
    constexpr int GHOSTLAKE = 32;
    constexpr int NOMAD = 33;
    constexpr int IBERIA = 34;
    constexpr int BRITAIN = 35;
    constexpr int MIDEAST = 36;
    constexpr int TEXAS = 37;
    constexpr int ITALY = 38;
    constexpr int CENTRALAMERICA = 39;
    constexpr int FRANCE = 40;
    constexpr int NORSELANDS = 41;
    constexpr int SEAOFJAPAN = 42;
    constexpr int BYZANTINUM = 43;
    constexpr int CUSTOM = 44;
    constexpr int BLINDRANDOM = 48;
    constexpr int ACROPOLIS = 49;
    constexpr int BUDAPEST = 50;
    constexpr int CENOTES = 51;
    constexpr int CITYOFLAKES = 52;
    constexpr int GOLDENPIT = 53;
    constexpr int HIDEOUT = 54;
    constexpr int HILLFORT = 55;
    constexpr int LOMBARDIA = 56;
    constexpr int STEPPE = 57;
    constexpr int VALLEY = 58;
    constexpr int MEGARANDOM = 59;
    constexpr int HAMBURGER = 60;
    constexpr int CTR_RANDOM = 61;
    constexpr int CTR_MONSOON = 62;
    constexpr int CTR_PYRAMIDDESCENT = 63;
    constexpr int CTR_SPIRAL = 64;
}

const std::map<int, ColorRGB> gaiaColors = {
    {units::GOLDMINE, {0xff, 0xc7, 0x00}},
    {units::STONEMINE, {0x91, 0x91, 0x91}},
    {units::CLIFF1, {0x71, 0x4b, 0x33}},
    {units::CLIFF2, {0x71, 0x4b, 0x33}},
    {units::CLIFF3, {0x71, 0x4b, 0x33}},
    {units::CLIFF4, {0x71, 0x4b, 0x33}},
    {units::CLIFF5, {0x71, 0x4b, 0x33}},
    {units::CLIFF6, {0x71, 0x4b, 0x33}},
    {units::CLIFF7, {0x71, 0x4b, 0x33}},
    {units::CLIFF8, {0x71, 0x4b, 0x33}},
    {units::CLIFF9, {0x71, 0x4b, 0x33}},
    {units::CLIFF10, {0x71, 0x4b, 0x33}},
    {units::RELIC, {0xff, 0xff, 0xff}},
    {units::TURKEY, {0xa5, 0xc4, 0x6c}},
    {units::SHEEP, {0xa5, 0xc4, 0x6c}},
    {units::DEER, {0xa5, 0xc4, 0x6c}},
    {units::BOAR, {0xa5, 0xc4, 0x6c}},
    {units::JAVELINA, {0xa5, 0xc4, 0x6c}},
    {units::FORAGEBUSH, {0xa5, 0xc4, 0x6c}}
};

namespace civs
{
    constexpr int NONE = 0;
    constexpr int BRITONS = 1;
    constexpr int FRANKS = 2;
    constexpr int GOTHS = 3;
    constexpr int TEUTONS = 4;
    constexpr int JAPANESE = 5;
    constexpr int CHINESE = 6;
    constexpr int BYZANTINES = 7;
    constexpr int PERSIANS = 8;
    constexpr int SARACENS = 9;
    constexpr int TURKS = 10;
    constexpr int VIKINGS = 11;
    constexpr int MONGOLS = 12;
    constexpr int CELTS = 13;
    constexpr int SPANISH = 14;
    constexpr int AZTECS = 15;
    constexpr int MAYANS = 16;
    constexpr int HUNS = 17;
    constexpr int KOREANS = 18;
    constexpr int ITALIANS = 19;
    constexpr int INDIANS = 20;
    constexpr int INCAS = 21;
    constexpr int MAGYARS = 22;
    constexpr int SLAVS = 23;
}