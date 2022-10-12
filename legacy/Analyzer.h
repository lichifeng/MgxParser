#pragma once

#include <map>
#include <algorithm>
#include <functional>
#include <sstream>
#include <iomanip>
#include <string>
#include <iostream>
#include <string.h>

#include "Record.h"
#include "Constants.h"
#include "DataStructures.h"
#include "EncodingConverter.h"

class Analyzer
{
    typedef bool (Analyzer::* MFP)();

public:

    Analyzer()
        :rec(nullptr), stream(nullptr), bodyVar({ 0,0,0,0,500,{0,0} }), _obj()
    {
        prepareHandlers();
    }

    ~Analyzer() = default;

    inline Analyzer& load(Record* r)
    {
        rec = r;
        return *this;
    }

    inline void parse()
    {
        current = (*(stream = &rec->_header)).begin();
        version();
        aoe2Header();
        AI();
        miscA();
        mapData();
        findKeyPos();
        scenarioInfo();
        readMessages(); // Encoding may be detected from now on
        victorySettings();
        miscB();
        playerMeta();
        skipTriggerInfo();
        teamInfo();
        miscC();
        pregameMsg();
        startInfo();

        current = (*(stream = &rec->_body)).begin();
        bodyCommands();
    }

private:
    Record*                     rec;
    std::vector<BYTE>*          stream;
    std::vector<BYTE>::iterator current;
    std::map<int8_t, MFP>       _objHandler, _cmdHandler;
    StartObject                 _obj;
    BodyVar                     bodyVar;

    bool loaded;

    inline void prepareHandlers()
    {
        _objHandler.insert(std::make_pair(UT_BUILDING,      &Analyzer::_readBuilding));
        _objHandler.insert(std::make_pair(UT_FLAG,          &Analyzer::_readFlag));
        _objHandler.insert(std::make_pair(UT_EYECANDY,      &Analyzer::_readEyeCandy));
        _objHandler.insert(std::make_pair(UT_DEAD_FISH,     &Analyzer::_readDeadOrFish));
        _objHandler.insert(std::make_pair(UT_BIRD,          &Analyzer::_readBird));
        _objHandler.insert(std::make_pair(UT_CREATABLE,     &Analyzer::_readCreatableUnit));
        _objHandler.insert(std::make_pair(0,                &Analyzer::_readZeroType));

        _cmdHandler.insert(std::make_pair(COMMAND_RESIGN,   &Analyzer::_cmdResign));
        _cmdHandler.insert(std::make_pair(COMMAND_RESEARCH, &Analyzer::_cmdResearch));
        _cmdHandler.insert(std::make_pair(COMMAND_TRAIN,    &Analyzer::_cmdTrain));
        _cmdHandler.insert(std::make_pair(COMMAND_TRIBUTE,  &Analyzer::_cmdTribute));
        _cmdHandler.insert(std::make_pair(COMMAND_POSTGAME, &Analyzer::_cmdPostgame));
    }

    template<typename T>
    inline T findPos(T haystackBeg, T haystackEnd, T needleBeg, T needleEnd)
    {
        return std::search(
            haystackBeg, haystackEnd,
            std::boyer_moore_searcher(
                needleBeg, needleEnd
            )
        );
    }

    /* Section A: Header parsers */
    /* Version Related Procedures */
    void            version();
    inline bool     _vercmp(const void* s, const char* pattern) const
    {
        return 0 == strcmp((char*)s, pattern);
    }
    void            _interpretVersion();


    void aoe2Header();

    void AI();

    void miscA();

    void mapData();

    void findKeyPos();

    void scenarioInfo();

    void victorySettings();

    void miscB();

    void playerMeta();

    void skipTriggerInfo();

    void readMessages();

    void teamInfo();

    void miscC();

    void pregameMsg();

    void startInfo();

    /* Section B: Body parsers */
    /* Step 14 */
    void bodyCommands();

    /* Substep 14-1 */
    void _processGameStart();

    /* Substep 14-2 */
    void _processChatMessage();

    void _fixEncoding();
    
    inline SPAN curPos() const { return current - stream->cbegin(); }

    inline void gotoPos(SPAN d) { current = stream->begin() + d; }

    inline void move(int d) { current += d; }

    inline void readBytes(SPAN n, void* dest)
    {
        memcpy(dest, &current[0], n);
        current += n;
    }

    inline void mark(BYTE*& x, int n = 0)
    {
        x = &current[0];
        current += n;
    }

    inline BYTE* curPtr(SPAN skip = 0)
    {
        BYTE* ptr = &current[0];
        current += skip;
        return ptr;
    }

    void handleObjs();
    inline bool isGaiaObj(int x) { return 1 == gaiaColors.count(x); }

    inline bool _objHandled()
    {
        readBytes(4, &_obj);
        return 1 == _objHandler.count(_obj.type) ?
            (this->*_objHandler[_obj.type])() : _defaultObjHandler();
    }

    inline bool handleCmd(int8_t cmd)
    {
        return 1 == _cmdHandler.count(cmd) ?
            (this->*_cmdHandler[cmd])() : _defaultCmdHandler();
    }

    inline bool _defaultObjHandler() { return false; } //TODO: log unknown type

    bool _readZeroType();
    bool _readBuilding();
    bool _readEyeCandy();
    bool _readFlag();
    bool _readDeadOrFish();
    bool _readBird();
    bool _readCreatableUnit();

    void handleOpChat();
    void handleOpSync();
    void handleOpViewlock();
    void handleOpCommand();
    
    inline bool _defaultCmdHandler() { return false; }

    bool _cmdResign();
    bool _cmdResearch();
    bool _cmdTrain();
    bool _cmdTribute();
    bool _cmdPostgame();

    inline void _readAoe2RecordString(std::string& str)
    {
        uint16_t len;
        readBytes(2, &len);
        str.assign((char*)&current[2], len);
        move(2 + len); // 0x60 0xa0
    }

    inline void _skipAoe2RecordString()
    {
        int16_t len;
        readBytes(2, &len);
        move(2 + len); // 0x60 0xa0
    }

    void _readPlayers1004(float version, int32_t playerCnt);

    void _guessEncoding();

    bool _findEncodingPattern(const char* pattern, std::string& mapName, size_t patternLen);

    inline void _readAoe2Guid()
    {
        std::stringstream ss;

        for (int i = 0; i < 16; i++, move(1))
        {
            ss << std::setfill('0') << std::setw(2) << std::hex << (current[0] | 0);
        }
        rec->aoe2header.guid.assign(ss.str());
    }

    inline std::string& _readString(std::string& dest)
    {
        uint16_t strLen;
        readBytes(2, &strLen);
        if (rec->rawEncoding.empty() || rec->outEncoding.empty())
        {
            dest.assign((char*)curPtr(strLen), strLen);
        }
        else
        {
            EncodingConverter conv(rec->outEncoding, rec->rawEncoding);
            conv.convert(std::string((char*)curPtr(strLen), strLen), dest);
        }
        return dest;
    }
};