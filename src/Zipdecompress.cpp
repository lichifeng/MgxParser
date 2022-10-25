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

//From old php version:
//// magic
//$head = unpack("Vsig/vver/vflag/vmeth/vmodt/vmodd/Vcrc/Vcsize/Vsize/vnamelen/vexlen", substr($data, 0, 30));
//$filename = substr($data, 30, $head['namelen']);
//$raw = gzinflate(substr($data, 30 + $head['namelen'] + $head['exlen'],
//$head['csize']));

#include <cstdint>
#include <string>
#include "string.h"
#include "Zipdecompress.h"

using namespace std;

#pragma pack(push) // 保存对齐状态
#pragma pack(2)    // 设定为1字节对齐
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

void fetchFromZip(const uint8_t *buf, ZipInfo *zipinfo, int wbits)
{
    MagicAssist *magicAssist;
    uint8_t zipSig[4] = {0x50, 0x4b, 0x03, 0x04};

    magicAssist = (MagicAssist *)buf;
    const uint8_t *cursor = buf;
    uintmax_t remain;
    cursor += 30;
    
    zipinfo->filename.resize(magicAssist->namelen);
    memcpy(zipinfo->filename.data(), cursor, magicAssist->namelen);
    cursor += (magicAssist->namelen + magicAssist->exlen);
    buf = cursor;

    if (0 != memcmp(&magicAssist->sig, zipSig, 4))
    {
        zipinfo->status = -1;
        return;
    }

    int ret;
    unsigned have;
    z_stream strm;
    uint32_t ZLIB_CHUNK = 512 * 1024;
    uint8_t in[ZLIB_CHUNK];
    uint8_t out[ZLIB_CHUNK];

    zipinfo->outBuffer.reserve(5 * 1024 * 1024);

    /* allocate inflate state */
    strm.zalloc = Z_NULL;
    strm.zfree = Z_NULL;
    strm.opaque = Z_NULL;
    strm.avail_in = 0;
    strm.next_in = Z_NULL;
    ret = inflateInit2(&strm, wbits);
    if (ret != Z_OK)
        zipinfo->status = ret;

    /* decompress until deflate stream ends or end of file */
    do
    {
        // if (FILE_INPUT == _inputType)
        // {
        //     _f.read((char *)&in, ZLIB_CHUNK);
        //     strm.avail_in = _f.gcount();
        //     if (!_f.good())
        //     {
        //         (void)inflateEnd(&strm);
        //         return Z_ERRNO;
        //     }
        // }
        // else
        // {
        //     strm.avail_in = filesize - (_curPos - _b) >= ZLIB_CHUNK ? ZLIB_CHUNK : filesize - (_curPos - _b);
        //     memcpy(&in, _curPos, strm.avail_in);
        //     _curPos += strm.avail_in;
        // }
        remain = magicAssist->csize - (cursor - buf);
        strm.avail_in = remain >= ZLIB_CHUNK ? ZLIB_CHUNK : remain;
        memcpy(&in, cursor, strm.avail_in);
        cursor += strm.avail_in;

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
                zipinfo->status = ret;
            }

            have = ZLIB_CHUNK - strm.avail_out;
            zipinfo->outBuffer.insert(
                zipinfo->outBuffer.end(),
                out,
                out + have);
            // if (!_f.good() && (FILE_INPUT == _inputType))
            // {
            //     (void)inflateEnd(&strm);
            //     return Z_ERRNO;
            // }
        } while (strm.avail_out == 0);

        /* done when inflate() says it's done */
    } while (ret != Z_STREAM_END && remain >0);

    /* clean up and return */
    (void)inflateEnd(&strm);
    zipinfo->status = ret == Z_STREAM_END ? Z_OK : Z_DATA_ERROR;
}