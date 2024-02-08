/***************************************************************
 * \file       cursor.cc
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#include "cursor.h"
#include "bytestohex.h"

RecCursor &RecCursor::operator()(std::size_t pos) {
    if (pos > rec_stream_.size())
        throw std::string("Cursor tried to land out of range.");
    current_ = rec_stream_.begin() + pos;
    return *this;
}

RecCursor &RecCursor::operator+=(long pos) {
    if (Remain() > RawStream().size() || Remain() < 0)
        throw std::string("Cursor tried to go out of range(operator+=).");
    current_ += pos;
    return *this;
}

RecCursor &RecCursor::operator-=(long pos) {
    return operator+=(pos * -1);
}

RecCursor &RecCursor::operator>>(std::string &s) {
    return ScanString(&s);
}

RecCursor &RecCursor::ScanString(std::string *s_ptr) {
    auto l = (uint16_t *) this->Ptr();
    uint16_t header_len, string_len;
    if (2656 == l[0]) { // de string
        string_len = l[1];
        header_len = 4;
    } else if (2656 == l[1]) { // hd string
        string_len = l[0];
        header_len = 4;
    } else if (0 == l[1]) { // pascal string with 4bytes header
        string_len = l[0];
        header_len = 4;
    } else { // pascal string with 2bytes header
        string_len = l[0];
        header_len = 2;
    }

    *this += header_len;
    // If meet an unusually long string, skip and try on.
    if (string_len > 3000)
        return *this;

    if (nullptr == s_ptr)
        return *this += string_len;

    // Assign bytes to string
    if (string_len && '\0' == this->Ptr()[string_len - 1])
        s_ptr->assign((const char *) this->Ptr(), string_len - 1);
    else
        s_ptr->assign((const char *) this->Ptr(), string_len);

    FixEncoding(*s_ptr);

    return *this += string_len;
}

RecCursor &RecCursor::Hex(std::string &buffer, int len, bool skip) {
    buffer = std::move(BytesToHex(this->Ptr(), len));

    if (skip)
        *this += len;

    return *this;
}

void RecCursor::FixEncoding(std::string &s) {
    if (!s.empty() && encoding_converter_) {
        try {
            encoding_converter_->Convert(s, s);
        } catch (...) {
            s = "<encoding error>";
        }
    }
}
