/***************************************************************
 * \file       addon_generatemap.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <cstdio>
#include <string>
#include <cstdlib>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>

#include "analyzer.h"
#include "map/map_parser.h"

// \todo 高版本的DE录像似乎不能得到正确的初始视角，需要修复

void DefaultAnalyzer::DrawMap(const std::string &save_path, uint32_t width, uint32_t height) {
    FILE *mapfile_ptr;
    mapfile_ptr = fopen(save_path.c_str(), "wb");
    if (mapfile_ptr == NULL) {
        throw std::string("Failed to create map file.");
    }
    DrawMap(mapfile_ptr, width, height);
    fclose(mapfile_ptr);
}

void DefaultAnalyzer::DrawMap(FILE *dest, uint32_t width, uint32_t height) {
    Analyze2Map();

    bool hd = (map_type_ == MgxParser::HD_MAP);
    if (7 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, DETile1>(dest, this, width, height, hd);
    } else if (9 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, DETile2>(dest, this, width, height, hd);
    } else if (4 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, Tile1>(dest, this, width, height, hd);
    } else if (2 == maptile_type_) {
        return ParseMapTile<DefaultAnalyzer, TileLegacy>(dest, this, width, height, hd);
    } else {
        throw std::string("Unknown map tile type.");
    }
}

std::string DefaultAnalyzer::DrawMap(uint32_t width, uint32_t height) {
    char* buffer;
    size_t buffer_size;
    FILE* stream = open_memstream(&buffer, &buffer_size);

    if (stream == NULL) {
        throw std::string("Failed to create base64 encoded map data.");
    }

    DrawMap(stream, width, height);
    fclose(stream);

    BIO *bio, *b64;
    BUF_MEM *bufferPtr;

    b64 = BIO_new(BIO_f_base64());
    BIO_set_flags(b64, BIO_FLAGS_BASE64_NO_NL);
    bio = BIO_new(BIO_s_mem());
    bio = BIO_push(b64, bio);

    BIO_write(bio, buffer, buffer_size);
    BIO_flush(bio);
    BIO_get_mem_ptr(bio, &bufferPtr);
    BIO_set_close(bio, BIO_NOCLOSE);
    BIO_free_all(bio);

    std::string base64Text(bufferPtr->data, bufferPtr->length);

    free(buffer);

    return base64Text;
}