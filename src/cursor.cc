#include "cursor.h"

RecCursor &RecCursor::operator()(std::size_t pos) {
    if (pos >= rec_stream_.size())
        throw "Cursor tried to go out of range.";
    current_ = rec_stream_.begin() + pos;
    return *this;
}

RecCursor &RecCursor::operator+=(long pos) {
    current_ += pos;
    if (current_ > rec_stream_.end() || current_ < rec_stream_.begin())
        throw "Cursor tried to go out of range with '+='.";
    return *this;
}

RecCursor &RecCursor::operator-=(long pos) {
    return operator+=(pos * -1);
}

RecCursor &RecCursor::operator>>(std::string &s) {
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

    // Assign bytes to string
    if ('\0' == this->Ptr()[string_len - 1])
        s.assign((const char *) this->Ptr(), string_len - 1);
    else
        s.assign((const char *) this->Ptr(), string_len);

    *this += string_len;

    if (!s.empty() && encoding_converter_) {
        try {
            encoding_converter_->convert(s, s);
        } catch (...) {
            s = "<encoding error>";
        }
    }

    return *this;
}
