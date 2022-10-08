/**
 * \file       DataModel.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-04
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#pragma once

#define UINT32_INIT 4294967295
#define FLOAT_INIT -20000.0
#define UINT8_INIT 255
#define RAW_ENCODING "GB18030"

#include <string>
#include <map>
#include "nlohmann_json_3/json.hpp"
#include "CompileConfig.h"
#include "Player.h"
#include "Helper.h"
#include "Chat.h"
#include "Lang/zh.h"

using namespace std;
using json = nlohmann::json;

class DataModel
{
public:
    /**
     * \brief      Read corresponding language string from a language file
     *
     * \param      l                   Language file
     * \param      i                   String index
     * \param      default             Fallback string if not found
     * \return     string              Translated string
     */
    string readLang(const map<uint32_t, string> &l, uint32_t i, string d = "-")
    {
        if (!(l.find(i) == l.end()))
        {
            return l.at(i);
        }
        else
        {
            return d;
        }
    }

    string toJson();

    // File-related members
    string filename;        ///< 录像的文件名
    string ext;             ///< 录像的扩展名 \todo 要检查扩展名
    uintmax_t filesize = 0; ///< 录像文件大小，单位是 bytes

    // Some config parameters
    uint16_t trailBytes = 5;               ///< 设定用于startinfo中玩家信息搜索时的特征字节长度，影响速度
    uint32_t easySkipBase = 35100;         ///< 在startinfo中搜索时可以放心跳过的字节长度
    uint32_t triggerStartSearchRange = 19; ///< 查找triggerinfo位置时的参数，较早版本有0和1，DE中一般为11，如果找不到可以考虑放大范围试试
    uint32_t ZLIB_CHUNK = 8192;            ///< ZLIB解压时的参数
    //uint32_t scenarioSearchSpan = 10000;    ///< usually 5500~6500

    // Version-related members
    uint32_t logVersion = UINT32_INIT;     ///< body 的前4个字节，与版本有关，可以识别A/C版
    char versionStr[8] = "";      ///< 代表游戏版本的原始字符串
    float saveVersion = FLOAT_INIT;       ///< \warning float有精度，进行比较的时候注意要合理处理，比如>11.76要写成>11.7599这种
    VERSIONCODE versionCode = UNSUPPORTED; ///< 这是自己定义的一个值，用于简化版本判断
    uint32_t includeAI;

    // HD/DE-specific data from header stream
    uint32_t DE_build = UINT32_INIT;           ///< In recent(2022-10) steam version of DE: 66692
    uint32_t DE_timestamp = 0;                 ///< 游戏时间，只有DE版本中有
    float DD_version = FLOAT_INIT;             ///< hd<=4.7: 1000; hd=5.8: 1006
    uint32_t DD_internalVersion = UINT32_INIT; ///< DE中是1000
    uint32_t DD_gameOptionsVersion = UINT32_INIT;
    uint32_t DD_DLCCount = 0;
    uint32_t DD_datasetRef; ///< \todo What's this?
    uint32_t DD_difficultyID = UINT32_INIT;
    uint32_t DD_selectedMapID;
    uint32_t DD_resolvedMapID;
    uint32_t revealMap = UINT32_INIT; ///< 0x00:通常，0x01:已开发，0x02:全部显示, 0x03:no fog
    uint32_t DD_victoryTypeID = UINT32_INIT;
    uint32_t DD_startingResourcesID = UINT32_INIT;
    uint32_t DD_startingAgeID;
    uint32_t DD_endingAgeID;
    uint32_t DD_gameType = UINT32_INIT; ///< Only when DD_version>=1006
    string HD_ver1000MapName;
    // string HD_ver1000Unknown;
    float DD_speed = FLOAT_INIT;
    uint32_t DD_treatyLength;
    // uint32_t DD_populationLimit = UINT32_INIT; ///< Store this value in populationLimit
    uint32_t DD_numPlayers = UINT32_INIT; ///< \note Gaia not included
    uint32_t DD_unusedPlayerColor;
    uint32_t DD_victoryAmount = UINT32_INIT;
    uint8_t DD_tradeEnabled;
    uint8_t DD_teamBonusDisabled;
    uint8_t DD_randomPositions;
    uint8_t DD_allTechs;
    uint8_t DD_numStartingUnits;
    uint8_t DD_lockTeams;
    uint8_t DD_lockSpeed;
    uint8_t DD_multiplayer;
    uint8_t DD_cheats = 255;
    uint8_t DD_recordGame;
    uint8_t DD_animalsEnabled;
    uint8_t DD_predatorsEnabled;
    uint8_t DD_turboEnabled;
    uint8_t DD_sharedExploration;
    uint8_t DD_teamPositions;
    uint32_t DD_subGameMode;
    uint32_t DD_battleRoyaleTime;
    uint8_t DD_handicap = UINT8_INIT; ///< 让分

    array<Player, 9> players;
    // uint8_t DE_fogOfWar;
    uint8_t DD_cheatNotifications;
    uint8_t DD_coloredChat;
    uint8_t DD_isRanked = 255;
    uint8_t DD_allowSpecs;
    uint32_t DD_lobbyVisibility;
    uint8_t DE_hiddenCivs;
    uint8_t DE_matchMaking;
    uint32_t DE_specDely;
    uint8_t DE_scenarioCiv;
    // string DE_RMSCrc;
    uint32_t HD_customRandomMapFileCrc;
    string HD_customScenarioOrCampaignFile;
    string HD_customRandomMapFile;
    string HD_customRandomMapScenarionFile;
    string DD_guid;
    string DD_lobbyName;
    string DD_moddedDataset;
    string HD_moddedDatasetWorkshopID;
    uint64_t DE_numAIFiles;

    // data from replay section
    uint32_t gameSpeed; ///< \todo If de/hd, use data from de/hd-specific data
    uint16_t recPlayer; ///< \todo index or number of pov?? verify this.
    uint8_t numPlayers; ///< \todo gaia included, DD_numPlayers first??
    uint8_t instantBuild;
    uint8_t cheatsEnabled = 255;
    // uint16_t gameMode;     ///< \note Unknown.

    // data from map data section
    int32_t mapCoord[2];
    void *mapDataPtr;
    uint8_t allVisible;
    // uint8_t fogOfWar; ///< \note Use fogOfWar in lobby

    // data from start info
    uint32_t restoreTime;

    // data from scenario header
    float scenarioVersion = FLOAT_INIT;
    string scenarioFilename;
    string instructions;

    // victory conditions
    uint32_t victoryIsConquest;
    uint32_t victoryRelics;
    uint32_t victoryExplored;
    uint32_t victoryAnyOrAll;
    uint32_t victoryMode;
    uint32_t victoryScore;
    uint32_t victoryTime;

    // game settings
    uint32_t mapID;
    uint32_t difficultyID = UINT32_INIT;
    uint32_t lockTeams;

    // lobby settings
    uint32_t fogOfWar;
    uint32_t mapSize;
    uint32_t populationLimit = UINT32_INIT;
    uint8_t gameType;
    uint8_t lockDiplomacy; ///< \note DE/HD数据中还有一个类似的
    uint8_t treatyLength;  ///< \note DE/HD数据中还有一个类似的
    vector<Chat> chat;
    int32_t DE_mapSeed;

    // other data
    string embededMapName; ///< Map name extracted from instructions, not mapped with raw number

    // data from body
    uint32_t duration = 0;
    uint32_t isMultiplayer;
    uint32_t syncChecksumInterval = 500;

    string rawEncoding = RAW_ENCODING; // \note GB18030是为兼容性考虑。这里rawEncoding不能和outEncoding一样，有地方会用这两个相等来路过转换
    string outEncoding = "utf-8";
    // string playDate;                       ///< 游戏发生时间，对老录像只能推断 \todo
    ///< 有时需要从上传时间来推断，是否放在更外层的类里面？
    string status = "good";                ///< 解析完成类型：good, warning, fatal, etc.
    string message;                        ///< 对 \p status 的具体说明
    string parseMode = "MgxParser Normal"; ///< 解析模式：normal, verbose, etc. 可以在命令行中指定
    double parseTime = 0;                  ///< 解析耗时（毫秒）
};
