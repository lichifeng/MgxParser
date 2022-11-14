/***************************************************************
 * \file       demo.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include <cstring>
#include <iostream>
#include <string>

#include "include/MgxParser.h"

int main(int argc, char *argv[]) {
    if (argc <= 1) {
        std::cout << "No Record Specified!" << std::endl;
        return 1;
    }

    int map_type = NO_MAP;
    bool extract = false;
    std::string file_path;
    for (size_t i = 0; i < argc; i++) {
        const char *argm = "-m";
        const char *argM = "-M";
        const char *arge = "-e";

        if (0 == strcmp(argm, argv[i]))
            map_type = NORMAL_MAP;
        else if (0 == strcmp(argM, argv[i]))
            map_type = HD_MAP;
        else if (0 == strcmp(arge, argv[i]))
            extract = true;
        else
            file_path.assign(argv[i]);
    }

    MgxParser::Settings _ = {.input_path = file_path,
                             .map_type = map_type,
                             .map_width = 900,
                             .map_height = 450,
                             .map_name = "map.png",
                             .extract_stream = extract};
    std::cout << MgxParser::parse(_) << std::endl;

    return 0;
}