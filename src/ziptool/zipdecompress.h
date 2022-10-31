#ifndef MGXPARSER_ZIPTOOL_H_
#define MGXPARSER_ZIPTOOL_H_

#include <cstdint>
#include <vector>

int ZipDecompress(uint8_t* stream, size_t stream_size, std::vector<uint8_t>& outbuffer);

#endif // !MGXPARSER_ZIPTOOL_H_