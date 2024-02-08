/***************************************************************
 * \file       analyzer.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <sstream>
#include "analyzer.h"

void DefaultAnalyzer::Run() {
    if (!ExtractStreams())
        throw std::string("Failed to assemble streams.");

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
    Analyze2Map();

    // ************
    // * Phase 2: *
    // ************
    //   Find some key positions
    //   2-1: Trigger info start position
    FindTrigger(6);

    //   2-2: Game settings start position. Need 2-1
    FindGameSettings(7);

    //   2-3：Disabled techs start position. Need 2-1
    FindDisabledTechs(8);

    //   2-4: Victory
    AnalyzeVictory(9);

    //   2-5: Find&Skip scenario data. Need victory start located.
    //   Nothing valuable here except a filename.
    //   What is really needed is instructions data after this section,
    //   which is key to detect file encoding.
    AnalyzeScenario(10);

    //   2-6: Messages, Instructions. Need 2-5
    AnalyzeMessages(11);

    //   2-7: Skip trigger info. Need 2-1
    //   This is useless data, but needed to reach lobby start.
    SkipTriggers(12);

    //   2-8: Game settings, player names first appears here (before HD/DE
    //   versions). Need 2-2
    AnalyzeGameSettings(13);

    //   2-9: Search initial player data postion. Need 2-5 & 2-8
    FindInitialDataPosition(14);

    // ************
    // * Phase 3: *
    // ************
    //   Do rest analyze. Following jobs is not necessarily ordered.

    //   3-1: Lobby data, lobby chat & some settings here. Need 2-7
    AnalyzeLobby(15);

    //   3-3: Go back to player initial data position and rummage some useful pieces
    AnalyzeInitialData(16);

    // ************
    // * Phase 4: *
    // ************
    // Analyze the body stream

    ReadBodyCommands(17);

    // ************
    // *  Addons  *
    // ************
    // Do some additional jobs.
    // \note Postgame data of UP14+ version will pop new data to players, 
    // so JudgeWinner() & CalcRetroGuid() must put behind ReadBodyCommands()
    JudgeWinner(18);
    CalcRetroGuid(19);
}

void DefaultAnalyzer::Analyze2Map() {
    if (MapReady())
        return;

    ExtractStreams();

    //   1-1: Version
    DetectVersion();

    //   1-2: HD/DE-specific data
    AnalyzeDEHeader(1);
    AnalyzeHDHeader(2);

    //   1-3: AI
    AnalyzeAi(3);

    //   1-4: Replay
    AnalyzeReplay(4);

    //   1-5: Map
    AnalyzeMap(5);
}