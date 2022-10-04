/**
 * \file       main.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      主程序入口，目前还没有开发完成。
 * \version    0.1
 * \date       2022-09-21
 *
 * \copyright Copyright (c) 2022
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
    logger.info("VersionString: {}", a.versionStr);
    logger.info("SaveVersion: {}", a.saveVersion);
    logger.info("DD_Version: {}", a.DD_version);
    logger.info("Int_Version: {}", a.DD_internalVersion);
    logger.info("DE_Build: {}", a.DE_build);
    logger.info("VersionCode: {}", a.versionCode);
    logger.info("Status: {}", a.status);
    logger.info("Duration: {}:{}:{}", a.duration / 1000 / 3600, a.duration / 1000 % 3600 / 60, a.duration / 1000 % 3600 % 60);
    logger.info("Parsing time: {:.2f}ms", logger.elapsed());
    a.extract("header.dat", "body.dat");
    logger.info("Parsing time + extract(): {:.2f}ms", logger.elapsed());
    a.generateMap("map.png", 360, 180);
    logger.info("Parsing time + extract() + map: {:.2f}ms", logger.elapsed());
    a.generateMap("HDmap.png", 1200, 600, true);
    logger.info("Parsing time + extract() + map + HDmap: {:.2f}ms", logger.elapsed());

    if (!DEBUG)
    {
        a.message = logger.dumpStr();
        cout << a.message << endl;
    }

    return 0;
}