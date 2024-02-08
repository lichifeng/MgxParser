/***************************************************************
 * \file       mainproc_loadfile.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "analyzer.h"
#include <filesystem>
#include <iterator>

bool DefaultAnalyzer::LoadFile() {
    auto inputpath = std::filesystem::path(inputpath_);
    input_filename_ = inputpath.filename().generic_string();
    input_ext_ = inputpath.extension().generic_string();

    if (!std::filesystem::exists(inputpath)) {
        Message("File not exists.");
        return false;
    }

    input_size_ = std::filesystem::file_size(inputpath);
    if (input_size_ < MIN_INPUT_SIZE) {
        Message("Filesize is too small.");
        return false;
    }

    input_file_.open(inputpath_, std::ios::in | std::ios::binary);

    if (!input_file_.is_open()) {
        Message("Filesize is too small.");
        return false;
    }

    // Stop eating new lines in binary mode!!!
    input_file_.unsetf(std::ios::skipws);

    // input_stream_.reserve(input_size_);
    // input_stream_.insert(
    //         input_stream_.begin(),
    //         std::istream_iterator<RECBYTE>(input_file_),
    //         std::istream_iterator<RECBYTE>());
    // ↑ VERY SLOW!
    // ↓ VERY FAST!
    input_stream_.resize(input_size_);
    input_file_.read((char*)input_stream_.data(), input_size_);

    input_file_.close();

    return true;
}