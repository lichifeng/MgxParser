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

void DefaultAnalyzer::extract(const string &headerPath, const string &bodyPath) {
    ofstream headerOut(headerPath, ofstream::binary);
    ofstream bodyOut(bodyPath, ofstream::binary);

    headerOut.write((char *)cursor_(0).Ptr(), body_start_);
    bodyOut.write((char *)cursor_(0).Ptr(), cursor_.RawStream().size() - body_start_);

    headerOut.close();
    bodyOut.close();
}