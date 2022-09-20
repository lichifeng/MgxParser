#pragma once

#include "Record.h"

#define AOCRecord Record

#define for_realms(rec,r) for(auto& r : rec.playerData.realms)if(r.valid())

#define for_players_in_realm(r,p) for(auto p : r.players)

#define for_players(rec,p) for(auto& p : rec.playerData.players)if(p.valid())

#define for_teams(rec,t) for(auto& t : rec.playerData.teams)if(t.valid())

#define for_players_in_team(t,p) for(auto p : t.players)

#define for_realms_in_team(t,r) int ____i = 0;for(Realm* r;____i < t.players.size() && (r = t.players[____i]->isFirstOfRealm);++____i)

#define for_pregame_messages(r,m) for(auto& m : rec.pregameMsg)

#define for_ingame_messages(r,m) for(auto& m : rec.ingameMsg)