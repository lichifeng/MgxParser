/**
 * \file       main.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-07
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include <exception>
#include <iostream>

#include "CompileConfig.h"
#include "Analyzers/default/Analyzer.h"
#include "Logger.h"

using namespace std;

int main(int argc, char *argv[])
{
    auto logger = Logger();

    if (DEBUG)
    {
        logger.warn("Debug Mode");
    }

    if (argc <= 1)
    {
        logger.fatal("No Record Specified!");
        return 1;
    }

    auto a = DefaultAnalyzer(argv[1]);
    a.logger = &logger;

    try
    {
        a.run();
    }
    catch (const exception &e)
    {
        logger.fatal("Exception: {}", e.what());
    }

    logger.info("Filename: {}", a.filename);
    logger.info("VersionCode: {}", a.versionCode);
    logger.info("Status: {}", a.status);
    logger.info("Duration: {}:{}:{}", a.duration / 1000 / 3600, a.duration / 1000 % 3600 / 60, a.duration / 1000 % 3600 % 60);
    logger.info("Parsing time: {:.2f}ms", a.parseTime = logger.elapsed());

    if (!DEBUG)
    {
        a.message = logger.dumpStr();
        cout << a.message << endl;
    }

    double t1, t2;
    cout << a.toJson() << endl;
    logger.info("json serialize time: {:.2f}ms", (t1 = logger.elapsed()) - a.parseTime);
    a.extract("header.dat", "body.dat");
    logger.info("extract() time: {:.2f}ms", (t2 = logger.elapsed()) - t1);
    a.generateMap("map.png", 360, 180);
    logger.info("map time: {:.2f}ms", (t1 = logger.elapsed()) - t2);
    a.generateMap("HDmap.png", 1200, 600, true);
    logger.info("HDmap time: {:.2f}ms", (t2 = logger.elapsed()) - t1);

    logger.info("dif: {}", a.difficultyID);
    logger.info("DDdif: {}", a.DD_difficultyID);
    logger.info("resources: {}", a.DD_startingResourcesID);
    logger.info("pop: {}", a.populationLimit);
    logger.info("ddcheats: {}, cheat:{}", a.DD_cheats, a.cheatsEnabled);
    logger.info("treatylen: {}", a.treatyLength);
    logger.info("mapid, selected, resolved: {}, {}, {}", a.mapID, a.DD_selectedMapID, a.DD_resolvedMapID);

    return 0;
}