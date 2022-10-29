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

bool DefaultAnalyzer::_loadFile()
{
    auto p = filesystem::path(path);

    if (!filesystem::exists(path))
    {
        _sendExceptionSignal(
            true,
            logger->fmt("File [{}] don't exist.", path));
    }

    filename = p.filename();
    ext = p.extension();
    filesize = filesystem::file_size(p);

    // Try open record file
    _f.open(path, ifstream::in | ifstream::binary);

    return _f.is_open();
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