#pragma once
#include <vector>
#include "Realm.h"
#include "Team.h"
#include "Player.h"

class PlayerData
{
public:
    /* get realm pointer by index */
    Realm* getRealm(int index)
    {
        if (index > 8 || index < 1)
            return nullptr;
        return realms[index - 1].valid() ? &realms[index - 1] : nullptr;
    }

    /* add a new realm */
    Realm& addRealm(int index)
    {
        realms[index - 1].index = index;
        return realms[index - 1];
    }

    /* get player pointer */
    Player* getPlayer(int number)
    {
        if (number > 8 || number < 1)
            return nullptr;
        return players[number - 1].valid() ? &players[number - 1] : nullptr;
    }
    
    /* add a new player */
    Player* addPlayer(
        int index, 
        int number, 
        bool isHuman = false, 
        bool isSpectator = false
    ) {
        Player& p = players[number - 1];
        p.number = number;
        p.isHuman = isHuman;
        p.isSpectator = isSpectator;

        Realm* r = getRealm(index);
        if (nullptr == r)
            return addRealm(index).addPlayer(p, true);
        else if (r->index > 0)
        {
            return r->addPlayer(p);
        }
        return nullptr; // TODO throw a exception
    }

    Realm realms[8];
    Team teams[12]; // 0~3: for team 1-4;4~11: for players not teamed
    Player players[8];
};