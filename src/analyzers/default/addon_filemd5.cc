/***************************************************************
 * \file       addon_filemd5.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/30
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <string>
#include <openssl/md5.h>
#include "analyzer.h"
#include "bytestohex.h"

std::string DefaultAnalyzer::CalcFileMd5(const uint8_t *input, std::size_t input_len) {

    unsigned char output_buf[MD5_DIGEST_LENGTH];

    MD5(input, input_len, output_buf);

    return std::move(BytesToHex(output_buf, MD5_DIGEST_LENGTH));
}