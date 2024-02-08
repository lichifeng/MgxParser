/***************************************************************
 * \file       zipdecompress.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_ZIPTOOL_H_
#define MGXPARSER_ZIPTOOL_H_

#include <cstdint>
#include <vector>

int ZipDecompress(uint8_t *stream, std::size_t stream_size, std::vector<uint8_t> &outbuffer);

#endif // !MGXPARSER_ZIPTOOL_H_