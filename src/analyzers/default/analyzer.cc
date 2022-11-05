/**
 * \file       analyzer.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-02
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include <sstream>

#include "analyzer.h"

using namespace std;

void DefaultAnalyzer::Run() {
    if (!status_.stream_extracted_ && !ExtractStreams())
        throw std::string("Unable to generate combined streams.");

    // Start data analyzing
    Analyze();
}

void DefaultAnalyzer::Analyze() {
    //              ┌───────────────┐
    //       ┌──────┤version        │
    //       │      ├───────────────┤   Inflated header data mainly
    //       │      │HD/DE header   │   composed of these parts.
    //       │      ├───────────────┤
    //       │      │AI data        │   Some of them can be accessed
    //       │      ├───────────────┤   by sequent read easily.
    //       │      │Replay         │
    //       │      ├───────────────┤   Some cannot. We need do some
    //       │      │Map            │   search to reach those parts.
    //       │      ├───────────────┤
    //       │      │Start info    ◄│   by lichifeng, 2022/10/1
    //    Inflated  ├───────────────┤
    //    header    │Achievement    │
    //    data      ├───────────────┤
    //    structure │Scenario      ◄│
    //       │      ├───────────────┤
    //       │      │Victory        │
    //       │      ├───────────────┤
    //       │      │Unknown        │
    //       │      ├───────────────┤
    //       │      │Disabled      ◄│
    //       │      ├───────────────│
    //       │      │Game settings ◄│
    //       │      ├───────────────│
    //       │      │Trigger info  ◄│
    //       │      ├───────────────┤
    //       └──────┤Lobby          │
    //              └───────────────┘

    // ************
    // * Phase 1: *
    // ************
    //   Reach sections which can reach easily(no search needed).
    //   1-1: Version
    DetectVersion();

    //   1-2: HD/DE-specific data
    AnalyzeDEHeader(4);
    AnalyzeHDHeader(5);

    //   1-3: AI
    AnalyzeAi(6);

    //   1-4: Replay
    AnalyzeReplay(7);

    //   1-5: Map
    AnalyzeMap(8);

    // ************
    // * Phase 2: *
    // ************
    //   Find some key positions
    //   2-1: Trigger info start position
    FindTrigger(10);

    //   2-2: Game settings start position. Need 2-1
    FindGameSettings(11);

    //   2-3：Disables start position. Need 2-1
    FindDisabledTechs(12);

    //   2-4: Victory
    AnalyzeVictory(21);

    //   2-5: Find&Skip scenario data. Need victory start detected.
    //   Nothing valuable here except a filename.
    //   What is really needed is instructions data after this section,
    //   which is key to detect file encoding.
    AnalyzeScenario(14);

    //   2-6: Messages, ie. Instructions. Need 2-5
    AnalyzeMessages(16);

    //   2-7: Skip trigger info. Need 2-1
    //   This is useless data, but need to get lobby start.
    SkipTriggers(17);

    //   2-8: Game settings part, player names first appears here (before HD/DE
    //   versions). Need 2-2
    AnalyzeGameSettings(18);

    //   2-9: Search initial player data postion. Need 2-5 & 2-8
    FindInitialDataPosition(19);

    throw std::string("Stopped under refactoring.");


    // ************
    // * Phase 3: *
    // ************
    //   Do rest analyze. Following jobs is not necessarily ordered.

    //   3-1: Lobby data, lobby chat & some settings here. Need 2-7
    _lobbyAnalyzer(20);

    //   3-3: Go back to player initial data position and rummage some useful pieces
    _startInfoAnalyzer(22);

    // ************
    // * Phase 4: *
    // ************
    // Analyze the body stream
    _switchStream(BODY_STRM);

    _readBodyCommands(23);

    // ************
    // *  Addons  *
    // ************
    // Do some additional jobs
    _guessWinner(24);
    _genRetroGuid(25);
}