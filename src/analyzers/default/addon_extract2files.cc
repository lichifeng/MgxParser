/***************************************************************
 * \file       addon_streamtofile.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/7
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "analyzer.h"

void DefaultAnalyzer::Extract2Files(const std::string &header_path, const std::string &body_path) {
    if (cursor_.RawStream().empty())
        return;

    std::ofstream headerOut(header_path, std::ofstream::binary);
    std::ofstream bodyOut(body_path, std::ofstream::binary);

    headerOut.write((char *)cursor_(0).Ptr(), body_start_);
    bodyOut.write((char *)cursor_(0).Ptr(), cursor_.RawStream().size() - body_start_);

    headerOut.close();
    bodyOut.close();
}