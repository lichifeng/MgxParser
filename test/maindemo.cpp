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
#include <string.h>
#include <iostream>
#include "../src/include/MgxParser.h"

using namespace std;

int main(int argc, char *argv[])
{
    if (argc <= 1)
    {
        cout << "No Record Specified!" << endl;
        return 1;
    }

    int mapType = NO_MAP;
    bool extractHB = false;
    string filepath;
    for (size_t i = 0; i < argc; i++)
    {
        const char *argm = "-m";
        const char *argM = "-M";
        const char *arge = "-e";

        if (0 == strcmp(argm, argv[i]))
            mapType = NORMAL_MAP;
        else if (0 == strcmp(argM, argv[i]))
            mapType = HD_MAP;
        else if (0 == strcmp(arge, argv[i]))
            extractHB = true;
        else
            filepath.assign(argv[i]);
    }

    std::string mapname = "map.png";
    cout << MgxParser::parse(std::move(filepath), mapType, std::move(mapname), extractHB) << endl;

    return 0;
}