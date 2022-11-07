#include "bytestohex.h"

/**
 * \brief      Use to convert bytes to hex string, mainly used to generate guid
 * in HD versions.
 *
 * \param      data                Normally a reference to _curPos(pointer to
 * current reading position). !! Must be a reference, otherwise skip will not work.
 * \param      len                 Bytes to convert
 * \param      skip                Skip read bytes (len bytes)
 * \return     std::string         Generated string representitive of bytes
 */
std::string BytesToHex(const unsigned char *data, int len) {
    constexpr char hexmap[] = {
            '0', '1', '2', '3', '4', '5', '6', '7',
            '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'
    };

    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i] = hexmap[(data[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[data[i] & 0x0F];
    }

    return s;
}