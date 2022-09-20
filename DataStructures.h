#pragma once
#include "Realm.h"

struct AOCUnit
{
    //int16_t unitID;
    Realm* owner;
    float x;
    float y;
};


struct BodyVar
{
    int32_t syncIndex;
    int32_t containsSequenceNumbers;
    int32_t numberOfChapters;
    int32_t currentTime; // in ms
    int32_t syncChecksumInterval = 500;
    int32_t syncData[2];
};

struct StartObject
{
    int8_t type;
    int8_t owner;
    uint16_t id;
    float pos[2]; //(x,y) of object

    inline bool isGate() // TODO: 这里还需要优化下，参考recanalyst
    { 
        return
            487 == id
            || 81 == id
            || 95 == id
            || 490 == id
            || 665 == id
            || 673 == id
            || 792 == id
            || 796 == id
            || 800 == id
            || 804 == id
            || 117 == id;
    }
};

struct ColorRGB
{
    unsigned char r;
    unsigned char g;
    unsigned char b;
};

struct IngameMsg
{
    uint32_t time;
    std::string msg;
};

struct Tribute
{
    Realm* from;
    Realm* to;
    int amount; // float in raw data stream
    int fee; // percentage, ie: 30 = 30%
    int32_t time;
    int8_t resourceID;
};

struct AOE2HEADER_PLAYER
{
    // TODO see aoc-mgz for DE player structure
    int32_t dataCrc;
    uint8_t mpVersion;
    int32_t teamIndex;
    int32_t civID;
    std::string AIBaseName;
    uint8_t AICivNameIndex;
    std::string unknownName;
    std::string playerName;
    int32_t humanity;
    uint64_t steamID;
    int32_t playerIndex;
    int32_t scenarioIndex;
};

struct AOE2HEADER
{
    float       version;
    uint32_t    internalVersion;
    uint32_t    gameOptionsVersion;
    uint32_t    DLCCnt;

    uint32_t    difficulty;
    uint32_t    mapSize;
    uint32_t    mapID;
    uint32_t    revealMap;
    uint32_t    victoryType;
    uint32_t    startingResources;
    uint32_t    startingAge;
    uint32_t    endingAge;
    uint32_t    gameType;

    float       gameSpeed;
    uint32_t    treatyLength;
    uint32_t    popLimit;
    uint32_t    numPlayers;
    uint32_t    unusedPlayerColor;
    uint32_t    victoryAmount;
    uint8_t     tradingEnabled;
    uint8_t     teamBonusesDisabled;
    uint8_t     randomizePositions;
    uint8_t     fullTechTreeEnabled;
    uint8_t     numberOfStartingUnits;
    uint8_t     teamsLocked;
    uint8_t     speedLocked;
    uint8_t     isMultiPlayer;
    uint8_t     cheatsEnabled;
    uint8_t     recordGameEnabled;
    uint8_t     animalsEnabled;
    uint8_t     predatorsEnabled;
    uint8_t     turboEnabled;
    uint8_t     sharedExploration;
    uint8_t     teamPositions;

    uint8_t     fogOfWarEnabled;
    uint8_t     cheatNotificationsEnabled;
    uint8_t     coloredChatEnabled;
    uint8_t     isRanked;
    uint8_t     allowSpectators;
    int32_t     lobbyVisibility;
    int32_t     customRandomMapFileCrc;

    std::string mapName;
    std::string customScenarioOrCampaignFile;
    std::string customRandomMapFile;
    std::string customRandomMapScenarioFile;
    std::string guid;
    std::string lobbyName;
    std::string moddedDatasetTitle;
    uint64_t moddedDatasetWorkshopId;
    std::vector<AOE2HEADER_PLAYER> players;
};

struct VictorySettings
{
    bool	customConquest;
    bool	customAll;
    int32_t customRelics;
    int32_t customPercentExplored;
    int32_t mode;
    int32_t score;
    int32_t timeLimit;
};