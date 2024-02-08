/***************************************************************
 * \file       chat.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_DATAMODEL_CHAT_H_
#define MGXPARSER_DATAMODEL_CHAT_H_

#include <string>

struct Chat {
    uint32_t time = 0;
    std::string msg;
};

#endif //MGXPARSER_DATAMODEL_CHAT_H_
