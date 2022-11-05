#include "cursor.h"

RecCursor &RecCursor::operator()(std::size_t pos) {
    if (pos >= rec_stream_.size())
        throw std::string("Cursor tried to go out of range.");
    current_ = rec_stream_.begin() + pos;
    return *this;
}

RecCursor &RecCursor::operator+=(long pos) {
    current_ += pos;
    if (current_ > rec_stream_.end() || current_ < rec_stream_.begin())
        throw std::string("Cursor tried to go out of range with '+='.");
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

    if (!s_ptr->empty() && encoding_converter_) {
        try {
            encoding_converter_->convert(*s_ptr, *s_ptr);
        } catch (...) {
            *s_ptr = "<encoding error>";
        }
    }

    return *this += string_len;
}

RecCursor &RecCursor::BytesToHex(std::string &buffer, int len, bool skip) {
    constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                               '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

    std::string s(len * 2, ' ');
    for (int i = 0; i < len; ++i) {
        s[2 * i] = hexmap[(current_[i] & 0xF0) >> 4];
        s[2 * i + 1] = hexmap[current_[i] & 0x0F];
    }
    buffer = std::move(s);
    if (skip)
        *this += len;
    return *this;
}
