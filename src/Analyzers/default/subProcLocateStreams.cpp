/**
 * \file       subProcLocateStreams.cpp
 * \author     PATRICK LI (admin@aocrec.com)
 * \brief
 * \version    0.1
 * \date       2022-10-25
 *
 * \copyright  Copyright (c) 2020-2022
 *
 */

#include "Analyzer.h"

bool DefaultAnalyzer::_locateStreams()
{
    _debugFlag = 3;

    // headerMeta[0]: header_len + next_pos + header_data (Data length)
    // headerMeta[1]: nextPos
    int32_t headerMeta[2];
    if (FILE_INPUT == _inputType)
        _f.read((char *)headerMeta, 8);
    else
        memcpy(headerMeta, _b, 8);

    // Examine header length
    if (headerMeta[0] < 4 || headerMeta[0] > filesize)
    {
        // Is this a zip archive? The first 4 bytes should be 50 4B 03 04,
        // that is 67324752 for uint32_t.
        if (67324752 != headerMeta[0])
            return false;

        // If this IS a zip archive, start unzip attemption
        _zipinfo = new ZipInfo();
        _zipinfo->rawSize = filesize;
        _zipinfo->status = 0;

        if (MEM_INPUT == _inputType)
        {
            fetchFromZipBuffer(_b, _zipinfo);
        }
        else
        {
            _f.seekg(ios_base::beg);
            fetchFromZipFile(_f, _zipinfo);
        }

        if (0 == _zipinfo->status)
        {
            filetype = "zip";
            extractedName = _zipinfo->filename;
            _b = _zipinfo->outBuffer.data();
            filesize = _zipinfo->outBuffer.size();
            _inputType = ZIP_INPUT;

            // ofstream zipout(extractedName, ofstream::binary);
            // zipout.write((char *)_b, filesize);
            // zipout.close();

            return _locateStreams();
        }
        else
        {
            _sendFailedSignal(true);
            if (-1 != _zipinfo->status)
                logger->fatal("ERRCODE:{}: Failed to unzip {}.", _zipinfo->status, path.empty() ? filename : path);
            return false;
        }
    }

    // Construct body stream
    _bodySize = filesize - headerMeta[0];
    _body.resize(_bodySize); /// \note vector 的 resize 和 reserve 是不一样的，这里因为这个问题卡了很久。reserve 并不会初始化空间，因此也不能直接读数据进去。
    if (FILE_INPUT == _inputType)
    {
        _f.seekg(headerMeta[0]);
        _f.read((char *)_body.data(), _bodySize);
    }
    else
    {
        memcpy(_body.data(), _b + headerMeta[0], _bodySize);
    }

    size_t rawHeaderPos = headerMeta[1] < filesize ? 8 : 4;

    if (rawHeaderPos == 4)
        versionCode = AOK;

    if (FILE_INPUT == _inputType)
    {
        _f.seekg(rawHeaderPos);
        return 0 == zipDecompress(&_f, FILE_INPUT, filesize, _header);
    }
    else
    {
        _curPos = _b + rawHeaderPos;
        return 0 == zipDecompress((void *)_curPos, MEM_INPUT, filesize, _header);
    }

    return true;
}