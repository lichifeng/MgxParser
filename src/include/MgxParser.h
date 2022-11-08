/***************************************************************
 * \file       MgxParser.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#ifndef MGXPARSER_H_
#define MGXPARSER_H_

#define NO_MAP 0
#define NORMAL_MAP 1
#define HD_MAP 2
#define MAP_NAME "minimap.png"

#include <string>

namespace MgxParser {
    /**
     * Parse a data stream. Used to parse uploaded files as buffer in memory.
     * @param input          Pointer to input stream
     * @param input_size     In bytes
     * @param map_type       NO_MAP(Default), NORMAL_MAP, HD_MAP
     * @param map_name       Name of generated map file
     * @param extract_stream Extract header&body streams to files. Default filenames: header.dat, body.dat
     * @return               A json string contains parsed results.
     */
    std::string parse(
            const uint8_t *input,
            size_t input_size,
            int map_type = NO_MAP,
            std::string map_name = MAP_NAME,
            bool extract_stream = false
    );

    /**
     * Parse a record file. Also accept a .zip archive containing a valid record as its first file.
     * @param input_path     Path to input file
     * @param map_type       NO_MAP(Default), NORMAL_MAP, HD_MAP
     * @param map_name       Name of generated map file
     * @param extract_stream Extract header&body streams to files. Default filenames: header.dat, body.dat
     * @return               A json string contains parsed results.
     */
    std::string parse(
            std::string input_path,
            int map_type = NO_MAP,
            std::string map_name = MAP_NAME,
            bool extract_stream = false
    );

    /**
     * To be used in building a python extension, not tested.
     * @param input_path     Path to input file. A C-style string.
     * @param map_type       NO_MAP(Default), NORMAL_MAP, HD_MAP
     * @param map_name       Name of generated map file
     * @param extract_stream Extract header&body streams to files. Default filenames: header.dat, body.dat
     * @return               A json string(C-style string) contains parsed results.
     */
    extern "C" const char *pyparse(
            const char *input_path,
            int map_type = NO_MAP,
            const char *map_name = MAP_NAME,
            bool extract_stream = false
    );
}

#endif // MGXPARSER_H_