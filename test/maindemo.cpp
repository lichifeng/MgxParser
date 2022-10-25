/**
 * \file       maindemo.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief      
 * \version    0.1
 * \date       2022-10-24
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */

#include <string>
#include <iostream>
#include "../src/Analyzers/default/Analyzer.h"
#include "../src/include/MgxParser.h"

using namespace std;

int main(int argc, char* argv[])
{
    if (argc <= 1)
    {
        cout << "No Record Specified!" << endl;
        return 1;
    }

    auto a = DefaultAnalyzer(argv[1]);

    try
    {
        a.run();
    }
    catch (const exception &e)
    {
        a.logger->fatal("Exception@_debugFlag#{}: {}", a.getDebugFlag(), e.what());
    }

    a.parseTime = a.logger->elapsed();
    a.message = a.logger->dumpStr();

    //a.generateMap("testmap.exe.png", 900, 450, true);

    cout << a.toJson() << endl;

    return 0;
}