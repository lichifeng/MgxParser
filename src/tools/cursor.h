/***************************************************************
 * \file       cursor.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_CURSOR_H_
#define MGXPARSER_CURSOR_H_

#define RECSTREAM std::vector<uint8_t>

#include <vector>
#include <memory>
#include <cstring>
#include <string>
#include "searcher.h"
#include "encoding_converter.h"

class RecCursor {
public:
    explicit RecCursor(RECSTREAM &rec_stream)
            : rec_stream_(rec_stream), current_(rec_stream.begin()) {};

    // Go to position
    RecCursor &operator()(std::size_t pos);

    RecCursor &operator()(RECSTREAM::iterator &itr) {
        current_ = itr;
        return *this;
    }

    // Go forward/backward by n
    RecCursor &operator+=(long pos);

    RecCursor &operator-=(long pos);

    inline RecCursor &operator--() { return *this -= 1; }

    // Get pointer/interator
    inline uint8_t *Ptr() { return &current_[0]; }

    inline RECSTREAM::iterator Itr() { return current_; }

    inline RECSTREAM::iterator Itr(size_t pos) { return rec_stream_.begin() + pos; }

    inline RECSTREAM::iterator Itr(RECSTREAM::iterator itr) { return current_ = itr; }

    inline RECSTREAM::reverse_iterator RItr(size_t pos) {
        return std::make_reverse_iterator(Itr(pos));
    }

    // Read bytes into variable
    template<typename T>
    RecCursor &operator>>(T &v) {
        auto T_size = sizeof(T);
        if ((*this)() + T_size > rec_stream_.size())
            throw std::string("Cursor go out of range when reading into a variable.");

        memcpy(&v, &current_[0], T_size);
        current_ += T_size;
        return *this;
    }

    // Peek a value
    template<typename T>
    inline T Peek(int skip = 0) { return *(T *) (Ptr() + skip); }

    RecCursor &operator>>(std::string &s);

    RecCursor &Hex(std::string &buffer, int len, bool skip = true);

    // Read/Skip string
    RecCursor &ScanString(std::string *s_ptr = nullptr);

    inline RecCursor &operator>>=(long n) {
        *this += n;
        return *this;
    }

    // skip n bytes, alias to >>=, useful when chaining
    inline RecCursor &operator>>(long &&n) {
        return *this >>= n;
    }

    // Tell current offset
    inline std::size_t operator()() { return current_ - rec_stream_.begin(); }

    // Remain bytes
    inline long Remain() { return rec_stream_.end() - current_; }

    // Test following bytes
    inline bool Test(void *p, uint32_t len) { return 0 == memcmp(&current_[0], p, len); }

    inline bool TestAndSkip(void *p, uint32_t len) {
        if (Test(p, len)) {
            current_ += len;
            return true;
        } else { return false; }
    }

    // get underlying stream
    inline RECSTREAM &RawStream() { return rec_stream_; }

    // set encoding converter
    inline void SetEncoding(const std::string &in, const std::string &out) {
        encoding_converter_ = std::make_unique<EncodingConverter>(out, in, true);
    }

    void FixEncoding(std::string &s);

private:
    RECSTREAM &rec_stream_;
    RECSTREAM::iterator current_;
    std::unique_ptr<EncodingConverter> encoding_converter_;
};

#endif //MGXPARSER_CURSOR_H_
