/***************************************************************
 * \file       bytestohex.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2022
 ***************************************************************/
 
#ifndef MGXPARSER_BYTESTOHEX_H_
#define MGXPARSER_BYTESTOHEX_H_

#include <string>

std::string BytesToHex(const unsigned char *data, int len);

#endif //MGXPARSER_BYTESTOHEX_H_
