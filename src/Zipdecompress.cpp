/**
 * \file       Zipdecompress.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-25
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include <cstdint>
#include <string>
#include <fstream>
#include "string.h"
#include "Zipdecompress.h"

#define ZLIB_CHUNK 256 * 1024

using namespace std;

#pragma pack(push) // 保存对齐状态
#pragma pack(2)    // 设定为2字节对齐
/**
 * \brief      Header of a zip file. https://docs.fileformat.com/compression/zip/
 */
struct MagicAssist
{
    uint32_t sig;
    uint16_t ver;
    uint16_t flag;
    uint16_t meth;
    uint16_t modt;
    uint16_t modd;
    uint32_t crc;
    uint32_t csize;
    uint32_t size;
    uint16_t namelen;
    uint16_t exlen;
};                ///< 30bytes total
#pragma pack(pop) // 恢复对齐状态

void fetchFromZipFile(ifstream &f, ZipInfo *z)
{
    MagicAssist magicAssist;

    f.read((char *)&magicAssist, 30);
    z->filename.resize(magicAssist.namelen);
    f.read(z->filename.data(), magicAssist.namelen);
    f.seekg(magicAssist.exlen, ios_base::cur);

    z->status = zipDecompress((void *)&f, 1, z->rawSize, z->outBuffer);
}

void fetchFromZipBuffer(const uint8_t *b, ZipInfo *z)
{
    MagicAssist *magicAssist;

    magicAssist = (MagicAssist *)b;
    const uint8_t *cursor = b;
    cursor += 30;

    z->filename.resize(magicAssist->namelen);
    memcpy(z->filename.data(), cursor, magicAssist->namelen);
    cursor += (magicAssist->namelen + magicAssist->exlen);
    b = cursor;

    z->status = zipDecompress((void *)b, 2, z->rawSize, z->outBuffer);
}

/**
 * \brief      This function uses zlib to inflate compressed data. Used to extract deflated header data in record file
 * and fetch a record file from a .zip archive.
 *
 * \param      src                 Pointer to a compressed data stream. Can be a Byte(uint8_t) array or a ifstream.
 * \param      srcType             1 for ifstream; 2 for byte array.
 * \param      srcSize             Size in bytes of compressed data.
 * \param      outBuffer           Destination of decompressed data. Reference to a uint8_t vector.
 * \return     int                 0 if sucessfully decompressed.
 */
int zipDecompress(void *src, int srcType, uint32_t srcSize, vector<uint8_t> &outBuffer)
{
    // Some settings
    uint32_t outReserve = 5 * 1024 * 1024;

    ifstream *f;
    const uint8_t *b;
    const uint8_t *c;

    int ret;
    unsigned have;
    z_stream strm;
    uint8_t in[ZLIB_CHUNK];
    uint8_t out[ZLIB_CHUNK];
    uint32_t remain;

    outBuffer.reserve(outReserve);

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, -MAX_WBITS);
    if (ret != Z_OK)
        return ret;

    switch (srcType)
    {
    case 1: // file input
        f = (ifstream *)src;
        break;

    case 2: // byte stream input
        b = c = (const uint8_t *)src;
        break;

    default:
        return 100; // invalid input type
    }

    /* decompress until deflate stream ends or end of file */
    do
    {
        if (1 == srcType) // File input
        {
            f->read((char *)&in, ZLIB_CHUNK);
            strm.avail_in = f->gcount();
            if (!f->good() && !f->eof())
            {
                (void)inflateEnd(&strm);
                return Z_ERRNO;
            }
        }
        else
        {
            remain = srcSize - (c - b);
            strm.avail_in = remain >= ZLIB_CHUNK ? ZLIB_CHUNK : remain;
            memcpy(&in, c, strm.avail_in);
            c += strm.avail_in;
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
            outBuffer.insert(
                outBuffer.end(),
                out,
                out + have);
            if ((0 == srcType) && !f->good() && !f->eof()) // file input
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