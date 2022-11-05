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

#include <filesystem>
#include <sstream>

#include "analyzer.h"
#include "utils.h"
#include "MapTools/TileStructures.h"

#define STOP_ON_FAILURE \
    if (_failedSignal)  \
        return;
#define TRY_PHASE2_FALLBACK \
    if (_failedSignal)      \
        goto PHASE2_FALLBACK;

using namespace std;

void DefaultAnalyzer::run() {
    if (!status_.stream_extracted_ && !ExtractStreams())
        throw std::string("Unable to generate combined streams.");

    // Start data analyzing
    _analyze();

    // Note encoding error
    /*if (_encodingError)
        logger_->warn("Encoding is not correct, need fix.");*/
}

void DefaultAnalyzer::_analyze() {
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

    throw std::string("Stopped under refactoring.");

    //   1-6: Find Startinfo
    _findStartInfoStart(9);
    STOP_ON_FAILURE

    // ************
    // * Phase 2: *
    // ************
    //   Find some key positions
    //   2-1: Trigger info start position
    _findTriggerInfoStart(10);
    STOP_ON_FAILURE

    //   2-2: Game settings start position. Need 2-1
    _findGameSettingsStart(11);
    STOP_ON_FAILURE

    //   2-3：Disables start position. Need 2-1
    _findDisablesStart(12);
    TRY_PHASE2_FALLBACK

    //   2-4: Skip victory-related data. Need 2-3
    _findVictoryStart(13);
    TRY_PHASE2_FALLBACK

    //   2-5: Find&Skip scenario data. Need 2-4
    //   Nothing valuable here except a filename.
    //   What is really needed is instructions data after this section,
    //   which is key to detect file encoding.
    _findScenarioHeaderStart(14);
    TRY_PHASE2_FALLBACK

    _scenarioHeaderAnalyzer(15);
    TRY_PHASE2_FALLBACK

    //   2-6: Messages, ie. Instructions. Need 2-5
    _messagesAnalyzer(16);
    // TRY_PHASE2_FALLBACK

    PHASE2_FALLBACK:
    //   2-7: Skip trigger info. Need 2-1
    //   This is useless data, but need to get lobby start.
    _triggerInfoAnalyzer(17);

    //   2-8: Game settings part, player names first appears here (before HD/DE
    //   versions). Need 2-2
    _gameSettingsAnalyzer(18);

    //   2-9: Search initial player data postion. Need 2-5 & 2-8
    _findInitialPlayersDataPos(19);

    // ************
    // * Phase 3: *
    // ************
    //   Do rest analyze. Following jobs is not necessarily ordered.

    //   3-1: Lobby data, lobby chat & some settings here. Need 2-7
    _lobbyAnalyzer(20);

    //   3-2: Victory
    _victorySettingsAnalyzer(21);

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

bool DefaultAnalyzer::_expectBytes(const vector<uint8_t> &pattern, bool skip) {
    bool passed = _bytecmp(_curPos, pattern.data(), pattern.size());
    if (passed && skip)
        _curPos += pattern.size();
    return passed;
}