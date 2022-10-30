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
#include "analyzer.h"

bool DefaultAnalyzer::_loadFile()
{
    auto p = filesystem::path(inputpath_);

    if (!filesystem::exists(inputpath_))
    {
        _sendExceptionSignal(
            true,
            logger_->fmt("File [{}] don't exist.", inputpath_));
    }

    input_filename_ = p.filename().generic_string();
    input_size_ = filesystem::file_size(p);

    // Try open record file
    input_file_.open(inputpath_, ifstream::in | ifstream::binary);

    return input_file_.is_open();
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