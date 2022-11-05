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

#include "analyzer.h"

void DefaultAnalyzer::Extract2Files(const string &header_path, const string &body_path) {
    ofstream headerOut(header_path, ofstream::binary);
    ofstream bodyOut(body_path, ofstream::binary);

    headerOut.write((char *)cursor_(0).Ptr(), body_start_);
    bodyOut.write((char *)cursor_(0).Ptr(), cursor_.RawStream().size() - body_start_);

    headerOut.close();
    bodyOut.close();
}