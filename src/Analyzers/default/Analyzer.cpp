/**
 * \file       Analyzer.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief      
 * \version    0.1
 * \date       2022-09-22
 * 
 * \copyright  Copyright (c) 2020-2022
 * 
 */
#define ZLIB_CHUNK 8192

#include <filesystem>
#include "zlib.h"
#include "Analyzer.h"

using namespace std;

void DefaultAnalyzer::run() {
    auto p = filesystem::path(path);

    if (!filesystem::exists(path))
    {
        status = "fail";
        message.append(path + " does not exists. \n");
        throw(AnalyzerException(message));
    }

    filename = p.filename();
    ext = p.extension();
    filesize = filesystem::file_size(p);

    // Try open record file
    _f.open(path, ifstream::in | ifstream::binary);
    if (!_f.is_open())
    {
        status = "fail";
        message.append("Failed to open " + path + ". \n");
        throw(AnalyzerException(message));
    }
    
    // Try to extract header&body streams
    if (!_locateStreams())
    {
        status = "fail";
        message.append("Failed to unzip raw header data. \n");
        throw(AnalyzerException(message));
    }

    // Log current stage
    status = "good";
    message.append("Successfully extracted header&body streams from this file. Ready for data analyzing. \n");
    
}

bool DefaultAnalyzer::_locateStreams() {
    /**
     * \brief      headerMeta[0]: header_len + next_pos + header_data (Data
     * length) \n headerMeta[1]: nextPos
     *
     */
    int32_t headerMeta[2];
    _f.read((char*)headerMeta, 8);

    // Construct body stream
    _bodySize = filesize - headerMeta[0];
    _body.resize(_bodySize); ///< \note vector 的 resize 和 reserve 是不一样的，这里因为这个问题卡了很久。reserve 并不会初始化空间，因此也不能直接读数据进去。
    _f.seekg(headerMeta[0]);
    _f.read((char*)_body.data(), _bodySize);

    uintmax_t rawHeaderPos = headerMeta[1] < filesize ? 8 : 4;
    _f.seekg(rawHeaderPos);
    if (rawHeaderPos == 4) versionCode = AOK;

    // Try to unzip header data
    if (_inflateRawHeader() != 0) return false;

    // Start data analyzing
    _analyze();

    return true;
}

void DefaultAnalyzer::_analyze() {
    // Start data analyzing
    
    // Get logVersion
    _switchStream(BODY_STRM);
    _readBytes(4, &logVersion);
    _switchStream(HEADER_STRM);

    _readBytes(8, versionStr);
    _readBytes(4, &saveVersion);
    _setVersionCode();

    _readBytes(4, &indcludeAI);
}

int DefaultAnalyzer::_setVersionCode() {
    ///< \todo Every condition needs to be tested!
    if (_strequal(versionStr, "TRL 9.3")) {
        return versionCode == AOK ? AOKTRIAL : AOCTRIAL;
    }
    if (_strequal(versionStr, "VER 9.3")) return versionCode = AOK;
    if (_strequal(versionStr, "VER 9.4")) {
        if (logVersion == 3) return versionCode = AOC10;
        if (logVersion == 5 || saveVersion >= 12.9699) return versionCode = DE;
        if (saveVersion >= 12.4999) return versionCode = HD50;
        if (saveVersion >= 12.4899) return versionCode = HD48;
        if (saveVersion >= 12.3599) return versionCode = HD46;
        if (saveVersion >= 12.3399) return versionCode = HD43;
        if (saveVersion > 11.7601) return versionCode = HD;
        if (logVersion == 4) return versionCode = AOC10C;
        return versionCode = AOC;
    }
    if (_strequal(versionStr, "VER 9.5")) return versionCode = AOFE21;
    if (_strequal(versionStr, "VER 9.8")) return versionCode = USERPATCH12;
    if (_strequal(versionStr, "VER 9.9")) return versionCode = USERPATCH13;
    if (_strequal(versionStr, "VER 9.A")) return versionCode = USERPATCH14RC1;
    if (_strequal(versionStr, "VER 9.B")) return versionCode = USERPATCH14RC2;
    if (_strequal(versionStr, "VER 9.C") || _strequal(versionStr, "VER 9.D")) return versionCode = USERPATCH14;
    if (_strequal(versionStr, "VER 9.E") || _strequal(versionStr, "VER 9.F")) return versionCode = USERPATCH15;
    if (_strequal(versionStr, "MCP 9.F")) return versionCode = MCP;

    // If none above match:
    status = "fail";
    message.append("Detected unsupported game version.");
    versionCode = UNSUPPORTED;
    throw(AnalyzerException(message));
}

int DefaultAnalyzer::_inflateRawHeader()
{
    int ret;
    unsigned have;
    z_stream strm;
    uint8_t in[ZLIB_CHUNK];
    uint8_t out[ZLIB_CHUNK];

    _header.reserve(HEADER_INIT);

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    /* decompress until deflate stream ends or end of file */
    do {
        _f.read((char*)&in, ZLIB_CHUNK);
        strm.avail_in = _f.gcount();
        if (!_f.good()) {
            (void)inflateEnd(&strm);
            return Z_ERRNO;
        }
        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do {
            strm.avail_out = ZLIB_CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            // assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret) {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR;     /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }

            have = ZLIB_CHUNK - strm.avail_out;
            _header.insert(
                _header.end(),
                out,
                out + have
            );
            if (!_f.good()) {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END);

    /* clean up and return */
    (void)inflateEnd(&strm);
    return ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}

string DefaultAnalyzer::generateMap(const string& path, bool hd) {
    return path;
}

void DefaultAnalyzer::extract(
    const string& headerPath  = "header.dat", 
    const string& bodyPath    = "body.dat"
) const
{
    ofstream headerOut(headerPath, ofstream::binary);
    ofstream bodyOut(bodyPath, ofstream::binary);

    headerOut.write((char*)_header.data(), _header.size());
    bodyOut.write((char*)_body.data(), _body.size());
    
    headerOut.close();
    bodyOut.close();
}