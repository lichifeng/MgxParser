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

#include "Analyzers/default/Analyzer.h"

using namespace std;

int main(int argc, char *argv[])
{
    auto a = DefaultAnalyzer(argv[1]);

    if (DEBUG)
    {
        a.logger->warn("Debug Mode");
    }

    if (argc <= 1)
    {
        a.logger->fatal("No Record Specified!");
        return 1;
    }

    try
    {
        a.run();
    }
    catch (const exception &e)
    {
        a.logger->fatal("Exception: {}", e.what());
    }

    if (!DEBUG)
    {
        a.message = a.logger->dumpStr();
    }

    cout << a.toJson() << endl;

    return 0;
}