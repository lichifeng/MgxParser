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
#include <cstdio>

namespace MgxParser {
/**
 * Settings for parser
 * @param input_stream   Pointer to input stream
 * @param input_path     Path to input file
 * @param input_size     In bytes
 * @param map_type       NO_MAP(Default), NORMAL_MAP, HD_MAP
 * @param map_width      Map width
 * @param map_height     Map heigth
 * @param map_name       Name of generated map file
 * @param map_dest       A C-style FILE handler to store map
 * @param extract_stream Extract header&body streams to files. Default filenames: header.dat, body.dat
 */
struct Settings {
    const uint8_t *input_stream = nullptr;
    std::string input_path;
    size_t input_size = 0;
    int map_type = NO_MAP;
    int map_width = 300;
    int map_height = 150;
    std::string map_name;
    FILE *map_dest = NULL;
    bool extract_stream = false;
    std::string header_path;
    std::string body_path;
    bool full_parse = true;
};

/**
 * Parse a data stream or a record file. Also accept a .zip archive containing a valid record as its first file.
 * @return               A json string contains parsed results.
 */
std::string parse(Settings &settings);

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
    bool extract_stream = false);
}  // namespace MgxParser

#endif  // MGXPARSER_H_