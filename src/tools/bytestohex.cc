/***************************************************************
 * \file       bytestohex.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/

#include "bytestohex.h"

/**
 * Use to convert bytes to hex string.
 * @param data  Pointer to source bytes
 * @param len   Length of bytes to be converted
 * @return      Generated string representitive of bytes
 */
std::string BytesToHex(const unsigned char *data, int len) {
    constexpr char hexmap[] = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }

    return s;
}