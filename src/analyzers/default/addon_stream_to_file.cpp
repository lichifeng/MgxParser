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

void DefaultAnalyzer::extract(const string &headerPath, const string &bodyPath) const {
    ofstream headerOut(headerPath, ofstream::binary);
    ofstream bodyOut(bodyPath, ofstream::binary);

    headerOut.write((char *) header_.data(), header_.size());
    bodyOut.write((char *) body_.data(), body_.size());

    headerOut.close();
    bodyOut.close();
}