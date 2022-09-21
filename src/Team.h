#pragma once
#include <vector>
#include "Player.h"

class Team
{
public:
    Team(): index(-1) {}

    int index;
    std::vector<Player*> players;

    inline bool valid() { return players.size(); }
};