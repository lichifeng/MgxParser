#include "CompileConfig.h"

#include <iostream>
#include "MgxParser.h"

using namespace std;

int main(int argc, char* argv[])
{

    if (argc <= 1)
    {
        cout << "No Record Specified!" << endl;
        return 1;
    }

    cout << argv[1] << endl;

    auto rec = AOCRecord();

    rec.outEncoding = "utf-8";
    
    for (int i = 0; i < 1; i++)
    {
        rec.load(argv[1]).parse(); // Normally, .parse() do not need to be call manually
        cout << i+1 << "th parsing finished in " << rec.parsingTime << "ms. " << endl;
    }

    //rec.extract("../rec.header", "../rec.body"); // Extract header and body streams to files

    //output = rec.exportJSON(VERBOSE);

    rec.getMap("map.png", 300, 150);
    rec.getHDMap("HDmap.png", 900, 450);

    cout << "RawEncoding: " << rec.outEncoding << endl;
    cout << "OutEncoding: " << rec.rawEncoding << endl;
    cout << "Parsing time: " << rec.parsingTime << endl;
    cout << "Version: " << rec.version << endl;
    

    for_realms(rec, r)
    {
        cout << "Realm " << r.index << "[Team" << r.team << "]: ";
        for_players_in_realm(r, p)
        {
            cout << p->name << "(Resigned: " << p->resignTime << ") ";
        }
        cout << endl;
    }

    for_players(rec, p)
    {
        cout << p.name << endl;
    }

    for_teams(rec, t)
    {
        for_realms_in_team(t, r)
        {
            cout << "Realm: " << r->index << endl;
        }
        for_players_in_team(t, p)
        {
            cout << "Team player: " << p->name << endl;
        }
    }

    cout << rec.messages["instructions"] << endl;
    
    cout << "\n-- Pregame Message: --\n" << endl;

    for_pregame_messages(rec, msg)
    {
        cout << msg << endl;
    }

    cout << "\n-- Ingame Message: --\n" << endl;

    for_ingame_messages(rec, msg)
    {
        cout << "(" << msg.time << ") " << msg.msg << endl;
    }

    cout << "Encoding: " << rec.rawEncoding << endl;
    cout << "MapName: " << rec.mapName << endl;

    cout << "Duration: " << rec.duration << endl;

    cout << "Mode: " << rec.getNvN() << endl;

    return 0;
}