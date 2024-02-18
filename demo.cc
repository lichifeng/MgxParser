/***************************************************************
 * \file       demo.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <cstring>
#include <iostream>
#include <string>
#include <cstdlib>
#include <unistd.h>
#include "include/MgxParser.h"


int main(int argc, char *argv[]) {
    MgxParser::MapType map_type = MgxParser::NO_MAP;
    std::string file_path;
    std::string unzip_filename;
    int json_indent = -1;

    int opt;
    while ((opt = getopt(argc, argv, "mMbBun:")) != -1) {
        switch (opt) {
            case 'm':
                map_type = MgxParser::NORMAL_MAP;
                break;
            case 'M':
                map_type = MgxParser::HD_MAP;
                break;
            case 'b':
                map_type = MgxParser::BASE64_NORMAL;
                break;
            case 'B':
                map_type = MgxParser::BASE64_HD;
                break;
            case 'u':
                unzip_filename = "original"; // use original filename in .zip archive
                break;
            case 'n':
                json_indent = std::atoi(optarg);
                if (json_indent <= 0) {
                    std::cerr << "Error: json_indent must be a number greater than 0.\n";
                    return 1;
                }
                break;
            default:
                std::cout << "Usage: ./mgxparser [options] [file]\n"
                          << "Options:\n"
                          << "  -m    Generate a normal map\n"
                          << "  -M    Generate a HD map\n"
                          << "  -b/-B Generate base64 encoded normal/HD mapdata\n"
                          << "  -u    Extract the original file in .zip archive\n"
                          << "  -n    Indentation of the json string. i.e. -n2\n"
                          << "  --help Display this help message\n"
                          << "Example: ./mgxparser -m -n4 demo.mgx\n"
                          << "Notice: if -b/-B is provided, -m/-M will be ignored.\n";
                return 1;
        }
    }

    if (optind < argc) {
        file_path.assign(argv[optind]);
    } else {
        std::cout << "No Record Specified!" << std::endl;
        return 1;
    }

    MgxParser::Settings _ = {.input_path = file_path,
                             .map_type = map_type,
                             .map_width = 900,
                             .map_height = 450,
                             .json_indent = json_indent};

    std::cout << MgxParser::parse(_) << std::endl;

    return 0;
}