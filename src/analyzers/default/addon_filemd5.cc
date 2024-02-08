/***************************************************************
 * \file       addon_filemd5.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/30
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <string>
#include "analyzer.h"
#include "md5/md5.h"
#include "bytestohex.h"

std::string DefaultAnalyzer::CalcFileMd5(const uint8_t *input, std::size_t input_len) {

    uint8_t output_buf[16];
    const uint8_t *output_ref = output_buf;
    MGXPARSER_MD5::MD5_CTX ctx;

    MGXPARSER_MD5::md5_init(&ctx);
    MGXPARSER_MD5::md5_update(&ctx, input, input_len);
    MGXPARSER_MD5::md5_final(&ctx, output_buf);

    return std::move(BytesToHex(output_ref, 16));
}