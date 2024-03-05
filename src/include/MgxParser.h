/***************************************************************
 * \file       MgxParser.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_H_
#define MGXPARSER_H_

#include <string>
#include <cstdio>
#include <cstdint>

namespace MgxParser
{
    /**
     * Map type
     * NO_MAP: Don't generate map
     * NORMAL_MAP: Generate a normal map
     * HD_MAP: Generate a high definition map based on the normal map
     */
    enum MapType
    {
        NO_MAP = 0,
        NORMAL_MAP,
        HD_MAP,
        BASE64_NORMAL,
        BASE64_HD
    };

    /**
     * Settings for parser
     * The parsing result is determined by the settings.
     * 
     * @param input_stream   Pointer to input stream. If not nullptr, the parser will use the stream instead of the file.
     * @param input_path     Path to input file. If input_stream is not NULL, this field will be ignored.
     * @param input_size     In bytes.
     * @param map_type       NO_MAP(Default), NORMAL_MAP, HD_MAP.
     * @param map_width      Map width. Default is 300.
     * @param map_height     Map heigth. Default is 150.
     * @param map_path       Path of generated map file.
     * @param map_dest       A C-style FILE handler to store map. If not NULL, the map will be written to the file.
     * @param header_path    If a path is provided, the header part of the record file will be extracted to the file. i.e. "header.dat".
     * @param body_path      If a path is provided, the body part of the record file will be extracted to the file. i.e. "body.dat".
     * @param full_parse     Parse the record file. Set to false if you only want to generate a map or extract raw data for record analyzing.
     * @param calc_md5       Calculate md5 of actuall record file(or data stream). It is useful when handling .zip archive, md5 of the real record file is different from the .zip file.
     * @param unzip          Extract record file from a .zip archive. If not empty, it should be a path to a .zip archive. Use "original" to retain the orignal filename in .zip archive. Use "buffer" to extract the record file to a buffer.
     * @param unzip_buffer   If you want to extract the record file to a buffer, provide a pointer to a buffer pointer. After parsing, the buffer will be allocated and the pointer will be assigned to it.
     * @param unzip_size_ptr Used to store the allocated buffer size. After parsing, the size of the buffer will be assigned to it. Required if unzip is "buffer".
     * @param json_indent    Indentation of the json string. -1 for no indentation.
     */
    struct Settings
    {
        const uint8_t *input_stream = nullptr;
        std::string input_path;
        size_t input_size = 0;
        MapType map_type = NO_MAP;
        unsigned map_width = 300;
        unsigned map_height = 150;
        std::string map_path;
        FILE *map_dest = NULL;
        std::string header_path;
        std::string body_path;
        bool full_parse = true;
        bool calc_md5 = true;
        std::string unzip;
        char **unzip_buffer = nullptr;
        std::size_t *unzip_size_ptr = nullptr;
        int json_indent = -1;
    };

    /**
     * Parse a data stream or a record file. Also accept a .zip archive containing a valid record as its first file.
     * Behavior of the parser is controlled by the settings, read the definition of Settings above for more details. 
     * 
     * @param s            Input settings
     * @return             A json string contains parsing results.
     */
    std::string parse(Settings &s);

} // namespace MgxParser

#endif // MGXPARSER_H_