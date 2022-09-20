#pragma once
#include <string>

class Realm;

class Player
{
public:
    Player()
        :realm(nullptr),
        number(-1),
        isHuman(false),
        isSpectator(false),
        resignTime(0),
        disconnected(false)
    {}

    int         number;
    Realm*      realm;
    bool        isHuman;
    bool        isSpectator;
    std::string name;
    uint32_t    resignTime;
    bool        disconnected;
    Realm*      isFirstOfRealm;

    inline bool valid() { return nullptr != realm; }
};

