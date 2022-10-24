/**
 * \file       subProcFileHandling.cpp
 * \author     PATRICK LI (lichifeng@qq.com)
 * \brief
 * \version    0.1
 * \date       2022-09-30
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "zlib.h"
#include "Analyzer.h"

void DefaultAnalyzer::_loadFile()
{
    auto p = filesystem::path(path);

    if (!filesystem::exists(path))
    {
        _sendFailedSignal(true);
        logger->fatal("{}(): File [{}] don't exist.", __FUNCTION__, path);
        return;
    }

    filename = p.filename();
    ext = p.extension();
    filesize = filesystem::file_size(p);

    // Try open record file
    _f.open(path, ifstream::in | ifstream::binary);
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
    do
    {
        if (FILE_INPUT == _inputType)
        {
            _f.read((char *)&in, ZLIB_CHUNK);
            strm.avail_in = _f.gcount();
            if (!_f.good())
            {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        }
        else
        {
            strm.avail_in = filesize - (_curPos - _b) >= ZLIB_CHUNK ? ZLIB_CHUNK : filesize - (_curPos - _b);
            memcpy(&in, _curPos, strm.avail_in);
            _curPos += strm.avail_in;
        }

        if (strm.avail_in == 0)
            break;
        strm.next_in = in;

        /* run inflate() on input until output buffer not full */
        do
        {
            strm.avail_out = ZLIB_CHUNK;
            strm.next_out = out;

            ret = inflate(&strm, Z_NO_FLUSH);
            // assert(ret != Z_STREAM_ERROR);  /* state not clobbered */
            switch (ret)
            {
            case Z_NEED_DICT:
                ret = Z_DATA_ERROR; /* and fall through */
            case Z_DATA_ERROR:
            case Z_MEM_ERROR:
                (void)inflateEnd(&strm);
                return ret;
            }

            have = ZLIB_CHUNK - strm.avail_out;
            _header.insert(
                _header.end(),
                out,
                out + have);
            if (!_f.good() && (FILE_INPUT == _inputType))
            {
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

void DefaultAnalyzer::extract(
    const string headerPath,
    const string bodyPath) const
{
    ofstream headerOut(headerPath, ofstream::binary);
    ofstream bodyOut(bodyPath, ofstream::binary);

    headerOut.write((char *)_header.data(), _header.size());
    bodyOut.write((char *)_body.data(), _body.size());

    headerOut.close();
    bodyOut.close();
}