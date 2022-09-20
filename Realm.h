#pragma once
#include <vector>
#include "Player.h"
#include "MapColors.h"

class Realm
{
public:
    Realm()
        : feudalTime(0),
        castleTime(0),
        imperialTime(0),
        team(-1),
        index(-1)
    {}

    int8_t civilization;
    int8_t color;
    int32_t index;
    int startInfoOffset;
    int team;

    int32_t feudalTime, castleTime, imperialTime;

    std::vector<std::pair<int32_t, uint16_t>> researches;
    
    struct InitState
    {
        float initCamera[2];
        float food;
        float wood;
        float stone;
        float gold;
        float headroom;
        float age;
        float houseCapacity;
        float population;
        float civilianPop;
        float militaryPop;
    } initState;

    std::vector<uint8_t> searchPattern;
    std::vector<Player*> players;

    inline Player* addPlayer(Player& p, bool isFirstOfRealm = false)
    {
        p.realm = this;
        if (isFirstOfRealm) p.isFirstOfRealm = this;
        players.push_back(&p);
        return players.back();
    }

    inline bool valid() const { return players.size(); }

    inline int getColor(int c) const
    {
        return playerColors[color][c];
    }
};