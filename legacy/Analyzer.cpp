#include <iostream>
#include <vector>
#include <typeinfo>
#include <map>
#include <string>
#include <sstream>
#include <iterator>

#include "Analyzer.h"
#include "Pattern.h"
#include "PlayerData.h"
#include "Player.h"
#include "Constants.h"
#include "Record.h"
#include "EncodingConverter.h"

using namespace std;

void Analyzer::version()
{
    readBytes(8, rec->rawVersion);
    rec->subVersion = *(float*)&current[0];
    move(4);
    _interpretVersion();
}

void Analyzer::_interpretVersion()
{
    rec->logVersion = rec->_body[0];

    if (_vercmp(rec->rawVersion, "TRL 9.3"))
    {
        rec->version = rec->isAOK ? "AOKTRIAL" : "AOCTRIAL";
        rec->isTrial = true; // TODO: AOK20/AOK20A? how to determine?
    }
    else if (_vercmp(rec->rawVersion, "VER 9.3"))
    {
        rec->version = "AOK";
        // isAOK should have been set to 1 here by Record::_getHeaderRaw()
        rec->isAOK = true;
    }
    else if (_vercmp(rec->rawVersion, "VER 9.4"))
    {
        rec->isAOC = rec->isHD = rec->isHDPatch4 = true;
        if (rec->subVersion >= 12.9699 || 5 == rec->logVersion)
            rec->version = "DE";
        else if (rec->subVersion >= 12.4999)
            rec->version = "HD50";
        else if (rec->subVersion >= 12.4899)
            rec->version = "HD48";
        else if (rec->subVersion >= 12.3599)
            rec->version = "HD46";
        else if (rec->subVersion >= 12.3399)
            rec->version = "HD43";
        else if (rec->subVersion > 11.7601)
        {
            rec->version = "HD";
            rec->isHDPatch4 = false; // subVersion >= 12 in hdpatch4
        }
        else
        {
            if (3 == rec->logVersion)
                rec->version = "AOC10A";
            else if (4 == rec->logVersion)
                rec->version = "AOC10C";
            else
                rec->version = "AOC";
            rec->isHD = rec->isHDPatch4 = false;
        }
    }
    else
    {
        rec->isAOC = rec->isUP = true;
        if (_vercmp(rec->rawVersion, "VER 9.5"))
            rec->version = "AOFE21";
        else if (_vercmp(rec->rawVersion, "VER 9.8"))
            rec->version = "USERPATCH12";
        else if (_vercmp(rec->rawVersion, "VER 9.9"))
            rec->version = "USERPATCH13";
        else if (_vercmp(rec->rawVersion, "VER 9.A"))
            rec->version = "USERPATCH14RC1";
        else if (_vercmp(rec->rawVersion, "VER 9.B"))
            rec->version = "USERPATCH14RC2";
        else if (_vercmp(rec->rawVersion, "VER 9.C") || _vercmp(rec->rawVersion, "VER 9.D"))
            rec->version = "USERPATCH14";
        else if (_vercmp(rec->rawVersion, "VER 9.E") || _vercmp(rec->rawVersion, "VER 9.F"))
        {
            rec->version = "USERPATCH15";
            rec->isUP15 = true;
        }
        else if (_vercmp(rec->rawVersion, "MCP 9.F"))
            rec->version = "MCP";
        else
            rec->version = rec->rawVersion; // TODO log and raise exception here
    }

    rec->isMsx = rec->subVersion >= 12.3199;
    rec->isAOE2 = rec->subVersion >= 12.3599;
}

void Analyzer::aoe2Header()
{
    if (!rec->isAOE2) return;

    readBytes(4, &rec->aoe2header.version);
    readBytes(4, &rec->aoe2header.internalVersion);
    readBytes(4, &rec->aoe2header.gameOptionsVersion);
    readBytes(4, &rec->aoe2header.DLCCnt);
    move(rec->aoe2header.DLCCnt * 4); // TODO record DLC ids??

    readBytes(4, &rec->aoe2header.difficulty);
    readBytes(4, &rec->aoe2header.mapSize);
    readBytes(4, &rec->aoe2header.mapID);
    readBytes(4, &rec->aoe2header.revealMap);
    readBytes(4, &rec->aoe2header.victoryType);
    readBytes(4, &rec->aoe2header.startingResources);
    readBytes(4, &rec->aoe2header.startingAge);
    readBytes(4, &rec->aoe2header.endingAge);
    readBytes(4, &rec->aoe2header.gameType);
    move(4);

    if (1000 == (int)rec->aoe2header.version)
    {
        _readAoe2RecordString(rec->aoe2header.mapName);
        _skipAoe2RecordString();
    }

    move(4);

    readBytes(4, &rec->aoe2header.gameSpeed);
    readBytes(4, &rec->aoe2header.treatyLength);
    readBytes(4, &rec->aoe2header.popLimit);
    readBytes(4, &rec->aoe2header.numPlayers);
    readBytes(4, &rec->aoe2header.unusedPlayerColor);
    readBytes(4, &rec->aoe2header.victoryAmount);
    move(4);

    readBytes(1, &rec->aoe2header.tradingEnabled);
    readBytes(1, &rec->aoe2header.teamBonusesDisabled);
    readBytes(1, &rec->aoe2header.randomizePositions);
    readBytes(1, &rec->aoe2header.fullTechTreeEnabled);
    readBytes(1, &rec->aoe2header.numberOfStartingUnits);
    readBytes(1, &rec->aoe2header.teamsLocked);
    readBytes(1, &rec->aoe2header.speedLocked);
    readBytes(1, &rec->aoe2header.isMultiPlayer);
    readBytes(1, &rec->aoe2header.cheatsEnabled);
    readBytes(1, &rec->aoe2header.recordGameEnabled);
    readBytes(1, &rec->aoe2header.animalsEnabled);
    readBytes(1, &rec->aoe2header.predatorsEnabled);
    readBytes(1, &rec->aoe2header.turboEnabled);
    readBytes(1, &rec->aoe2header.sharedExploration);
    readBytes(1, &rec->aoe2header.teamPositions);
    move(4);

    if (rec->aoe2header.version >= 1003.9999)
    {
        _readPlayers1004(rec->aoe2header.version, rec->aoe2header.numPlayers);
    }
    else
    {
        auto separatorPos = findPos(
            current, current + 1000, // is 1000 enough?
            pattern::aoe2headerSeparator.begin(),
            pattern::aoe2headerSeparator.end()
        );
        separatorPos = findPos(
            separatorPos + 4, current + 1000, // is 1000 enough?
            pattern::aoe2headerSeparator.begin(),
            pattern::aoe2headerSeparator.end()
        );
        if (current + 1000 == separatorPos)
        {
            // TODO throw exception
        }
        else
        {
            current = separatorPos + 4;
            current += 10;
        }
        return;
    }

    move(1);
    readBytes(1, &rec->aoe2header.fogOfWarEnabled);
    readBytes(1, &rec->aoe2header.cheatNotificationsEnabled);
    readBytes(1, &rec->aoe2header.coloredChatEnabled);

    move(4);

    readBytes(1, &rec->aoe2header.isRanked);
    readBytes(1, &rec->aoe2header.allowSpectators);

    readBytes(4, &rec->aoe2header.lobbyVisibility);
    readBytes(4, &rec->aoe2header.customRandomMapFileCrc);

    //for (int i = 0; i++ < 3; move(8)) _skipAoe2RecordString();
    _readAoe2RecordString(rec->aoe2header.customScenarioOrCampaignFile);
    move(8);
    _readAoe2RecordString(rec->aoe2header.customRandomMapFile);
    move(8);
    _readAoe2RecordString(rec->aoe2header.customRandomMapScenarioFile);
    move(8);

    _readAoe2Guid();
    _readAoe2RecordString(rec->aoe2header.lobbyName);
    _readAoe2RecordString(rec->aoe2header.moddedDatasetTitle);
    readBytes(8, &rec->aoe2header.moddedDatasetWorkshopId);
    move(8);

    if (rec->aoe2header.version >= 1004.9999)
    {
        _skipAoe2RecordString();
        move(4);
    }
    else if (rec->aoe2header.version >= 1003.9999)
    {
        move(8);
    }
}

void Analyzer::AI()
{
    rec->includeAI = (bool)*(uint32_t*)&current[0];
    move(4);

    if (!rec->includeAI) return;

    move(2);
    uint16_t numAIStrings = *(uint16_t*)&current[0];
    move(6);
    for (uint32_t i = 0; i < numAIStrings; i++)
    {
        move(*(int32_t*)&current[0] + 4);
    }
    move(6);

    int actionSize = 24; // See recanalyst
    int ruleSize = 16 * (actionSize + 1); // See recanalyst

    if (rec->isHDPatch4) ruleSize += 0x180;

    for (int16_t i = 0, numRules; i < 8; i++)
    {
        move(10);
        readBytes(2, &numRules);
        move(4);
        for (int j = 0; j++ < numRules; move(ruleSize));
    }
    move(5544);
    if (rec->subVersion >= 11.9599) move(1280);
    if (rec->subVersion >= 12.2999) move(4);
}

void Analyzer::miscA()
{
    move(4);
    if (rec->isAOE2)
    {
        move(4);
        // FIXME gameSpeed could be 175 and other, needs verification.
        rec->gameSpeed = rec->aoe2header.gameSpeed;
    }
    else
    {
        readBytes(4, &rec->gameSpeed);
    }

    move(37);


    readBytes(2, &rec->pov);
    if (rec->isAOE2)
    {
        rec->playerCnt = rec->aoe2header.numPlayers;
        rec->gameMode = rec->aoe2header.isMultiPlayer;
        move(5);
    }
    else
    {
        readBytes(1, &rec->playerCnt);
        --rec->playerCnt; // TODO: playercnt need --, #0 is GAIA
        if (rec->isAOC)
        {
            rec->isInstantBuilding = current++[0];
            rec->isCheatEnabled = current++[1];
        }
        readBytes(2, &rec->gameMode);
    }

    if (rec->subVersion >= 12.4999)
        move(12);

    if (rec->isAOE2 && rec->subVersion >= 12.4899)
        move(46);
    else
        move(58);
}

void Analyzer::mapData()
{
    readBytes(8, &rec->mapXY);
    if (rec->mapXY[0] >= 1000 || rec->mapXY[1] >= 1000)
    {
        // TODO give some hints for exceptions
        rec->mapXY[0] = rec->mapXY[1] = 0;
    }

    int32_t numMapZones, mapBits, numFloats;
    mapBits = rec->mapXY[0] * rec->mapXY[1];
    readBytes(4, &numMapZones);
    for (int i = 0; i < numMapZones; i++)
    {
        if (rec->subVersion >= 11.9299)
            move(2048 + mapBits * 2);
        else
            move(1275 + mapBits);
        readBytes(4, &numFloats);
        move(numFloats * 4 + 4);
    }

    rec->allVisible = current++[0];
    rec->fogOfWar = current++[0];
    mark(
        rec->_mapBitmap,
        mapBits * (((uint8_t)0xff == current[0]) ? 4 : 2)
    );

    int32_t numData, numObstructions;
    readBytes(4, &numData);
    move(4 + numData * 4);
    for (int i = 0; i < numData; i++)
    {
        readBytes(4, &numObstructions);
        move(numObstructions * 8);
    }

    int32_t visibilityMapSize[2];
    readBytes(8, visibilityMapSize);
    move(visibilityMapSize[0] * visibilityMapSize[1] * 4);
    move(4);
    readBytes(4, &numData);
    move(numData * 27);
    move(4); // int. Value is 10060 in AoK recorded games, 40600 in AoC and on.
}

void Analyzer::findKeyPos()
{
    vector<BYTE>::reverse_iterator rFound;

    rec->_startInfoPos = curPos();
    
    rFound = findPos(
        rec->_header.rbegin(),
        rec->_header.rend(), 
        pattern::gameSettingEnd.rbegin(),
        pattern::gameSettingEnd.rend()
    );
    rec->_triggerInfoPos = rec->_header.rend() - rFound; // TODO: what if not found?

    rFound = findPos(
        make_reverse_iterator(rec->_header.begin() + rec->_triggerInfoPos),
        rec->_header.rend(), 
        pattern::separator.rbegin(),
        pattern::separator.rend()
    );
    rec->_gameSettingPos = rec->_header.rend() - rFound;

    auto scenarioSeprator = rec->isAOK ? \
        pattern::aokSeparator : \
        (
            rec->isAOE2 ? \
            pattern::aoe2recordScenarioSeparator : \
            pattern::scenarioConstant
        );

    rFound = findPos(
        make_reverse_iterator(rec->_header.begin() + rec->_gameSettingPos),
        rec->_header.rend(),
        scenarioSeprator.rbegin(),
        scenarioSeprator.rend()
        
    );
    rec->_scenarioHeaderPos = rec->_header.rend() - rFound - 4 - scenarioSeprator.size();
}

void Analyzer::scenarioInfo()
{
    gotoPos(rec->_scenarioHeaderPos);
    move(4433);
    
    uint16_t scenarioFileNameLen;
    readBytes(2, &scenarioFileNameLen);
    if (scenarioFileNameLen <= 224)
    {
        rec->scenarioFilename = string((char*)curPtr(scenarioFileNameLen), scenarioFileNameLen);
        rec->gameType = TYPE_SCENARIO;
    }
}

void Analyzer::readMessages()
{
    string s;

    move(rec->isAOK ? 20 : 24);

    rec->messages.insert(make_pair("instructions", _readString(s)));
    _guessEncoding();
    if (!rec->outEncoding.empty()) {
        EncodingConverter conv(rec->outEncoding, rec->rawEncoding);
        conv.convert(rec->messages["instructions"], rec->messages["instructions"]);
    }

    rec->messages.insert(make_pair("hints",        _readString(s)));
    rec->messages.insert(make_pair("victory",      _readString(s)));
    rec->messages.insert(make_pair("loss",         _readString(s)));
    rec->messages.insert(make_pair("history",      _readString(s)));
    rec->messages.insert(make_pair("scouts",       _readString(s)));

    _fixEncoding();

    auto found = findPos(
        rec->_header.begin() + curPos(),
        rec->_header.begin() + rec->_gameSettingPos - 5456 - 12544,
        pattern::separator.begin(),
        pattern::separator.end()
    );
    found = findPos(
        found,
        rec->_header.begin() + rec->_gameSettingPos - 5456 - 12544,
        pattern::separator.begin(),
        pattern::separator.end()
    );
    gotoPos(found - rec->_header.begin());
}

void Analyzer::victorySettings()
{
    uint32_t tmpU;

    readBytes(4, &tmpU);
    rec->victory.customConquest = 0 == tmpU ? false : true;
    move(4);
    readBytes(4, &rec->victory.customRelics);
    move(4);
    readBytes(4, &rec->victory.customPercentExplored);
    move(4);
    readBytes(4, &tmpU);
    rec->victory.customAll = 0 == tmpU ? false : true;
    readBytes(4, &rec->victory.mode);
    readBytes(4, &rec->victory.score);
    readBytes(4, &rec->victory.timeLimit);
}

void Analyzer::miscB()
{
    gotoPos(rec->_gameSettingPos + 8);
    
    if (rec->subVersion >= 12.2999) move(16);

    if (rec->isAOE2)
    {
        move(4);
        rec->mapID = rec->aoe2header.mapID;
    }
    else if (!rec->isAOK)
    {
        readBytes(4, &rec->mapID);
    }
    readBytes(4, &rec->difficulty);
    rec->lockTeams = *(uint32_t*)curPtr(4) ? true : false;
}

void Analyzer::playerMeta()
{
    /*
    Read player info block in gamesetting section
    recanalyst�а�ͬһ����ɫ��һ�����ֵ������Ϊ����������û�в��ɣ�
    */

    int32_t index, playerType;
    uint32_t nameLen;
    PlayerOld* p;

    for (int i = 0; i < 9; i++)
    {
        readBytes(4, &index);
        readBytes(4, &playerType);
        readBytes(4, &nameLen); // TODO hd���е��ַ�����UTF8��Ҫ�ֿ�����
        
        switch (playerType)
        {
        case 0x02:
        case 0x04:
        case 0x06:
            p = rec->playerData.addPlayer(
                index,
                i,
                0x02 == playerType ? true : false,
                0x06 == playerType ? true : false
            );
            break;
        default:
            if (nameLen > 0)
                move(nameLen);
            continue;
        }

        // TODO: what if invalid nameLen?
        p->name.assign((char*)curPtr(nameLen), nameLen);

        // TODO: merge player data from aoe2record header

        if (p == p->realm->players.front())
        {
            p->realm->searchPattern.resize(++nameLen + 7);
            memcpy(&p->realm->searchPattern[0], &nameLen, 2);
            memcpy(&p->realm->searchPattern[2], p->name.c_str(), nameLen);
            memcpy(
                &p->realm->searchPattern[2 + nameLen],
                pattern::startInfoTrailSeparator.data(),
                5
            );
            
            auto searchEnd = rec->_header.begin() + rec->_scenarioHeaderPos - rec->playerCnt * 1817;
            auto found = findPos(
                rec->_header.begin() + rec->_startInfoPos + 18000,
                searchEnd,
                p->realm->searchPattern.begin(),
                p->realm->searchPattern.end()
            );
            p->realm->startInfoOffset = found == searchEnd ? \
                0 : found - rec->_header.begin() - rec->_startInfoPos + nameLen + 2;
        }

        if ((!rec->rawEncoding.empty()) && (!rec->outEncoding.empty()))
        {
            EncodingConverter conv(rec->outEncoding, rec->rawEncoding);
            conv.convert(p->name, p->name);
        }
    }
}

void Analyzer::skipTriggerInfo()
{
    int conditionSize = 72;
    int32_t
        numTriggers,
        descriptionLen,
        nameLen,
        numEffects,
        numSelectedObjs,
        textLen,
        soundFilenameLen,
        numConditions;
    
    gotoPos(rec->_triggerInfoPos + 1);

    if (rec->isHDPatch4) conditionSize += 8;

    readBytes(4, &numTriggers);
    for (int i = 0; i < numTriggers; i++)
    {
        move(18);
        readBytes(4, &descriptionLen);
        move(descriptionLen > 0 ? descriptionLen : 0);
        readBytes(4, &nameLen);
        move(nameLen > 0 ? nameLen : 0);
        readBytes(4, &numEffects);
        for (int i = 0; i < numEffects; i++)
        {
            move(24);
            readBytes(4, &numSelectedObjs);
            numSelectedObjs = numSelectedObjs == -1 ? 0 : numSelectedObjs;
            move(rec->isHDPatch4 ? 76 : 72);
            readBytes(4, &textLen);
            move(textLen > 0 ? textLen : 0);
            readBytes(4, &soundFilenameLen);
            move(soundFilenameLen > 0 ? soundFilenameLen : 0);
            move(numSelectedObjs * 4);
        }
        move(numEffects * 4);
        readBytes(4, &numConditions);
        move(numConditions * conditionSize + numConditions * 4);
    }

    if (numTriggers > 0) move(numTriggers * 4);
}

void Analyzer::teamInfo()
{
    int teamIndex;
    PlayerOld* player;
    for (int i = 1; i < 9; i++)
    {
        teamIndex = (int)current++[0] - 2; // raw range: 1~5, 1 -> no team

        if (teamIndex >= -1 && teamIndex <= 3 && (player = rec->playerData.getPlayer(i)))
        {
            if (teamIndex == -1) teamIndex = player->realm->index + 3;
            if (player->realm->team == -1) player->realm->team = teamIndex + 1;
            if (rec->playerData.teams[teamIndex].index == -1) rec->playerData.teams[teamIndex].index = teamIndex + 1;
            rec->playerData.teams[teamIndex].players.push_back(player);
        }
    }
}

void Analyzer::miscC()
{
    move(rec->subVersion < 12.2999 ? 1 : 0);

    readBytes(4, &rec->revealMap);

    move(4);

    readBytes(4, &rec->mapSize);
    readBytes(4, &rec->popLimit);
    if (rec->isUP) rec->popLimit *= 25;
    if (rec->isAOC)
    {
        rec->gameType = (int)current++[0];
        rec->lockDiplomacy = (bool)current++[0];
    }

    move(rec->subVersion >= 11.9599 ? 1 : 0);
    move(rec->isHD ? 4 : 0);
}

void Analyzer::pregameMsg()
{
    // ��������ܻẬ��ð��
    if (rec->isAOC)
    {
        int32_t msgCnt, msgLen;
        string tmpMsg;

        readBytes(4, &msgCnt);
        msgCnt = msgCnt > PREMSG_MAX ? PREMSG_MAX : msgCnt;

        for (int i = 0; i < msgCnt; i++)
        {
            readBytes(4, &msgLen);
            if (msgLen <= 0) continue;

            if (
                '@' == current[0]
                && '#' == current[1]
                && '1' <= current[2]
                && '8' >= current[2]
            )
            {
                if (rec->rawEncoding.empty() || rec->outEncoding.empty())
                {
                    tmpMsg = string((char*)curPtr(msgLen), msgLen);
                }
                else
                {
                    EncodingConverter conv(rec->outEncoding, rec->rawEncoding);
                    conv.convert(string((char*)curPtr(msgLen), msgLen), tmpMsg);
                }
                
                rec->pregameMsg.push_back(tmpMsg);
            }
            else
            {
                move(msgLen);
            }
        }
    }
}

void Analyzer::startInfo()
{
    vector<BYTE>::iterator resourcesEnd;
    int32_t numResources;
    int8_t civilization;

    // Handle Gaia Objects
    gotoPos(rec->_startInfoPos);
    move(976); // 976 = 1 + 4 + 1 + 792 + 41 * 4 + 9 + 5
    move(rec->isAOC ? 9 : 5);
    move(rec->isTrial ? 4 : 0);
    move(rec->playerCnt + 70);
    move(rec->isAOC ? 42041 : 35033);
    move(rec->mapXY[0] * rec->mapXY[1]);
    handleObjs();

    // Handle PlayerOld Objects
    for(auto& realm : rec->playerData.realms)
    {

        if (realm.valid() && realm.startInfoOffset)
        {
            gotoPos(rec->_startInfoPos + realm.startInfoOffset);
            move(1);
            readBytes(4, &numResources);
            move(1);
            resourcesEnd = current + 4 * numResources;

            // TODO: Some data
            readBytes(4, &realm.initState.food);
            readBytes(4, &realm.initState.wood);
            readBytes(4, &realm.initState.stone);
            readBytes(4, &realm.initState.gold);
            readBytes(4, &realm.initState.headroom);
            move(4);
            readBytes(4, &realm.initState.age);
            move(16);
            readBytes(4, &realm.initState.population);
            move(100);
            readBytes(4, &realm.initState.civilianPop);
            move(8);
            readBytes(4, &realm.initState.militaryPop);
            realm.initState.houseCapacity = 
                realm.initState.population + realm.initState.headroom;

            current = resourcesEnd;
            move(1);
            readBytes(8, &realm.initState.initCamera);
            move(rec->isAOC ? 9 : 5);

            readBytes(1, &civilization);
            if (!realm.civilization)
                realm.civilization = civilization ? civilization : 1;
            move(3);
            readBytes(1, &realm.color);

            move(rec->isTrial ? 4 : 0);
            move(rec->playerCnt + 70);
            move(rec->isAOC ? 42041 : 35033);
            move(rec->mapXY[0] * rec->mapXY[1]);

            handleObjs();
        }
    }
}

void Analyzer::handleObjs()
{
    auto searchEnd = rec->_header.begin() + rec->_scenarioHeaderPos - 1817;
    auto found = findPos(
        current,
        searchEnd,
        pattern::existObjectSeparator.begin(),
        pattern::existObjectSeparator.end()
    );
    if (found == searchEnd) return;
    current = found + pattern::existObjectSeparator.size();

    while (_objHandled());
}

bool Analyzer::_readZeroType()
{
    move(-4);
    if (
        0 == memcmp(
            &current[0],
            pattern::playerInfoEndSeparator.data(),
            pattern::playerInfoEndSeparator.size()
        )
    ) return false;

    if (
        current[0] == pattern::objectsMidSeparatorGaia[0]
        && current[1] == pattern::objectsMidSeparatorGaia[1]
        ) move(pattern::objectsMidSeparatorGaia.size());
    else return false;

    return true;
}

bool Analyzer::_readBuilding()
{
    if (rec->playerData.getRealm(_obj.owner))
    {
        move(19);
        readBytes(8, _obj.pos);
        rec->_playerObjs.push_back(_obj);
    }

    auto searchEnd = rec->_header.begin() + rec->_scenarioHeaderPos - 1817;
    auto searchPattern = rec->isAOC ?
        pattern::objectEndSeparator
        : pattern::aokObjectEndSeparator;
    auto found = findPos(
        current,
        searchEnd,
        searchPattern.begin(),
        searchPattern.end()
    );
    if (found == searchEnd) return false; //TODO: log it.
    current = found + searchPattern.size();

    move(126);
    if (rec->isAOC) move(1);
    if (rec->isHDPatch4) move(-4);

    return true;
}

bool Analyzer::_readEyeCandy()
{
    // Track GAIA objects for the map image generator.
    if (isGaiaObj(_obj.id))
    {
        move(19);
        readBytes(8, _obj.pos);

        rec->_gaiaObjs.push_back(_obj);

        move(-19 - 8);
    }
    move(59);

    if (rec->isHD) move(3);
    if (rec->subVersion >= 12.4899) move(4);
    if (rec->isAOK) move(1);

    return true;
}

bool Analyzer::_readFlag()
{
    if (rec->isHD) move(3);
    if (rec->isAOC)
    {
        move(59);
        move(2 == current[0] ? 39 : 5);
    }
    else
    {
        move(99);
    }

    return true;
}

bool Analyzer::_readDeadOrFish()
{
    if (rec->isHD) move(3);
    if (rec->subVersion >= 12.4899) move(4);
    if (!rec->isAOC) move(1);

    if (2 == current[59]) move(17);
    move(200);
    if (rec->isHDPatch4) move(1);

    return true;
}

bool Analyzer::_readBird()
{
    int8_t b;
    move(203);
    readBytes(1, &b);
    move(229);
    if (b) move(67);

    return true;
}

bool Analyzer::_readCreatableUnit()
{
    if (isGaiaObj(_obj.id))
    {
        move(19);
        readBytes(8, _obj.pos); //(X,Y) 2FLOATs of gaia object
        rec->_gaiaObjs.push_back(_obj);
    }
    else if (rec->playerData.getRealm(_obj.owner))
    {
        move(19);
        readBytes(8, _obj.pos); //TODO: (X,Y) 2FLOATS OF OBJECT
        rec->_playerObjs.push_back(_obj);
    }

    auto searchEnd = rec->_header.begin() + rec->_scenarioHeaderPos - 1817;
    auto searchPattern = rec->isAOC ?
        pattern::objectEndSeparator
        : pattern::aokObjectEndSeparator;
    auto found = findPos(
        current,
        searchEnd,
        searchPattern.begin(),
        searchPattern.end()
    );
    if (found == searchEnd) return false; //TODO: log it.
    current = found + searchPattern.size();

    return true;
}

void Analyzer::handleOpChat()
{
    readBytes(4, &bodyVar.syncChecksumInterval);
    if (-1 == bodyVar.syncChecksumInterval)
    {
        _processChatMessage();
    }
    else
    {
        move(-4);
        _processGameStart();
    }
}

void Analyzer::handleOpSync()
{
    ++bodyVar.syncIndex;
    readBytes(8, bodyVar.syncData);
    bodyVar.currentTime += bodyVar.syncData[0];
    move(bodyVar.syncIndex == bodyVar.syncChecksumInterval ? 28 : 0);
    move(12); // TODO: �������Ϣ�������ã��������ӽǵ��ƶ�
}

void Analyzer::handleOpViewlock()
{
    move(12); // TODO: ���������ӽ��л���������簴H�е�TC����(4byte float, 4byte float, 4byte int)
}

void Analyzer::handleOpCommand()
{
    int32_t cmdLen;
    vector<BYTE>::iterator pNext;
    int8_t cmd;

    readBytes(4, &cmdLen);

    pNext = (rec->_body.end() - current) < cmdLen ? rec->_body.end() : current + cmdLen;
    readBytes(1, &cmd);

    handleCmd(cmd);

    current = pNext;
}

bool Analyzer::_cmdResign()
{
    uint8_t resignData[3]; //{realm index, player number, disconnected}
    readBytes(3, resignData);
    {
        auto player = rec->playerData.getPlayer(resignData[1]);
        if (player)
        {
            if (player->resignTime == 0) player->resignTime = bodyVar.currentTime;
            if (!player->disconnected) player->disconnected = (bool)resignData[2];
        }
    }
    return true;
}

bool Analyzer::_cmdResearch()
{
    move(7); // 3 + 4, 4 bytes is buildingID
    
    uint16_t index, researchID;
    readBytes(2, &index);
    readBytes(2, &researchID);
    
    Realm* r = rec->playerData.getRealm(index);
    if (!r) { return false; }

    switch (researchID)
    {
    case RESEARCH_FEUDAL:
        r->feudalTime = bodyVar.currentTime + 0.5 + 130000; // 130000: research duration
        break;
    case RESEARCH_CASTLE:
        r->castleTime = bodyVar.currentTime + 0.5 \
            + (civs::PERSIANS == r->civilization ? 160000 : 160000 / 1.10);
    case RESEARCH_IMPERIAL:
        r->imperialTime = bodyVar.currentTime + 0.5 \
            + (civs::PERSIANS == r->civilization ? 190000 : 190000 / 1.10);
    }

    r->researches.push_back(make_pair(bodyVar.currentTime, researchID));

    return true;
}

bool Analyzer::_cmdTrain()
{
    move(11);
    return true;
}

bool Analyzer::_cmdTribute()
{
    Tribute tribute;

    tribute.from = rec->playerData.getRealm(current[0]);
    tribute.to = rec->playerData.getRealm(current[1]);
    tribute.resourceID = current[2];
    tribute.time = bodyVar.currentTime;
    move(3);

    if (tribute.from && tribute.to)
    {
        tribute.amount = (int)(*(float*)&(*current));
        tribute.fee = (int)(100 * (*(float*)&current[4]));
        rec->tribute.push_back(tribute);
    }
    move(8);

    return true;
}

bool Analyzer::_cmdPostgame()
{
    //TODO: POSTGAMEDATA

    return true;
}

void Analyzer::_readPlayers1004(float version, int32_t playerCnt)
{
    AOE2HEADER_PLAYER p;

    for (int i = 0; i < 8; i++)
    {
        if (i >= playerCnt)
        {
            //skip empty players
            move(48);
            if (rec->aoe2header.version >= 1004.9999) move(4);
            continue;
        }

        move(2);
        
        readBytes(4, &p.dataCrc);
        readBytes(1, &p.mpVersion);
        readBytes(4, &p.teamIndex);
        readBytes(4, &p.civID);
        _readAoe2RecordString(p.AIBaseName);
        readBytes(1, &p.AICivNameIndex);
        if (rec->aoe2header.version >= 1004.9999)
            _readAoe2RecordString(p.unknownName);
        _readAoe2RecordString(p.playerName);
        readBytes(4, &p.humanity);
        readBytes(8, &p.steamID);
        readBytes(4, &p.playerIndex);
        move(4);
        readBytes(4, &p.scenarioIndex);

        rec->aoe2header.players.push_back(p);
    }
}

void Analyzer::bodyCommands()
{  
    int32_t opType;

    if (rec->isAOK) { _processGameStart(); }

    while (rec->_body.end() - current >= 3)
    {
        readBytes(4, &opType);
        switch (opType)
        {
        case OP_CHAT:
            handleOpChat();
            break;
        case OP_SYNC:
            handleOpSync();
            break;
        case OP_VIEWLOCK:
            handleOpViewlock();
            break;
        case OP_COMMAND:
            handleOpCommand();
            break;
        default:
            break;
        }
    }

    rec->duration = bodyVar.currentTime;
}

void Analyzer::_processGameStart()
{
    int32_t isMultiplayer;
    readBytes(4, &bodyVar.syncChecksumInterval);
    if (rec->isAOK)
    {
        move(33); // TODO: NOT TESTED, &ver in recanalyst
    }
    else
    {
        readBytes(4, &isMultiplayer);
        rec->isMultiplayer = (bool)isMultiplayer;
        move(8); // pov & revealMap, duplicated
        readBytes(4, &bodyVar.containsSequenceNumbers);
        readBytes(4, &bodyVar.numberOfChapters);
    }
}

void Analyzer::_processChatMessage()
{
    int32_t len;
    IngameMsg tmpMsg;

    readBytes(4, &len);
    if (len <= 0) return;

    if (
        '@' == current[0]
        && '#' == current[1]
        && '1' <= current[2]
        && '8' >= current[2]
    )
    {
        // TODO: Ҫ������������ð�ŵ����?
        if (
            '-' != current[3]
            || '-' != current[4]
            || '-' != current[len - 2]
            //|| '-' != current[len - 3]
        )
        {
            tmpMsg.time = bodyVar.currentTime;
            if (rec->rawEncoding.empty() || rec->outEncoding.empty())
            {
                tmpMsg.msg.assign((char*)curPtr());
            }
            else
            {
                EncodingConverter conv(rec->outEncoding, rec->rawEncoding);
                conv.convert(string((char*)curPtr(), len), tmpMsg.msg);
            }
            rec->ingameMsg.push_back(tmpMsg);
        }
    }
    move(len);
}

void Analyzer::_fixEncoding()
{

    if ((!rec->outEncoding.empty()) && (!rec->rawEncoding.empty()))
    {
        EncodingConverter conv(rec->outEncoding, rec->rawEncoding);
        conv.convert(rec->mapName, rec->mapName);
        conv.convert(rec->scenarioFilename, rec->scenarioFilename);
    }
}

void Analyzer::_guessEncoding()
{

    if (_findEncodingPattern(pattern::zh_pattern, rec->mapName, size(pattern::zh_pattern)))
    {
        rec->rawEncoding = "cp936";
    }
    else if (_findEncodingPattern(pattern::zh_utf8_pattern, rec->mapName, size(pattern::zh_utf8_pattern)))
    {
        rec->rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(pattern::zh_wide_pattern, rec->mapName, size(pattern::zh_wide_pattern)))
    {
        rec->rawEncoding = "cp936";
    }
    else if (_findEncodingPattern(pattern::zh_tw_pattern, rec->mapName, size(pattern::zh_tw_pattern)))
    {
        rec->rawEncoding = "cp950";
    }
    else if (_findEncodingPattern(pattern::br_pattern, rec->mapName, size(pattern::br_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(pattern::de_pattern, rec->mapName, size(pattern::de_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(pattern::en_pattern, rec->mapName, size(pattern::en_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(pattern::es_pattern, rec->mapName, size(pattern::es_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(pattern::fr_pattern, rec->mapName, size(pattern::fr_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(pattern::it_pattern, rec->mapName, size(pattern::it_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (_findEncodingPattern(pattern::jp_pattern, rec->mapName, size(pattern::jp_pattern)))
    {
        rec->rawEncoding = "cp932";
    }
    else if (_findEncodingPattern(pattern::jp_utf8_pattern, rec->mapName, size(pattern::jp_utf8_pattern)))
    {
        rec->rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(pattern::ko_pattern, rec->mapName, size(pattern::ko_pattern)))
    {
        rec->rawEncoding = "cp949";
    }
    else if (_findEncodingPattern(pattern::ko_utf8_pattern, rec->mapName, size(pattern::ko_utf8_pattern)))
    {
        rec->rawEncoding = "utf-8";
    }
    else if (_findEncodingPattern(pattern::ru_pattern, rec->mapName, size(pattern::ru_pattern)))
    {
        rec->rawEncoding = "windows-1251";
    }
    else if (_findEncodingPattern(pattern::ru_utf8_pattern, rec->mapName, size(pattern::ru_utf8_pattern)))
    {
        rec->rawEncoding = "windows-1251";
    }
    else if (_findEncodingPattern(pattern::nl_pattern, rec->mapName, size(pattern::nl_pattern)))
    {
        rec->rawEncoding = "windows-1252";
    }
    else if (rec->isHD)
    {
        rec->rawEncoding = "utf-8";
    }
    else if (rec->rawEncoding.size() == 0)
    {
        rec->rawEncoding = "gbk";
    }
}

bool Analyzer::_findEncodingPattern(const char* pattern, std::string& mapName, size_t patternLen)
{
    size_t pos, posEnd;

    if (string::npos != (pos = rec->messages["instructions"].find(pattern)))
    {
        posEnd = rec->messages["instructions"].find('\n', pos + patternLen);
        if (string::npos != posEnd)
            rec->mapName = rec->messages["instructions"].substr(pos + patternLen, posEnd - pos - patternLen);

        return true;
    }

    return false;
}
