/***************************************************************
 * \file       status.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include <string>

class Status {
public:
    int debug_flag_ = 0;
    bool input_loaded_ = false;
    bool stream_extracted_ = false;
    bool version_detected_ = false;
    bool encoding_detected_ = false;
    bool mapdata_found_ = false;
    bool body_scanned_ = false;
};