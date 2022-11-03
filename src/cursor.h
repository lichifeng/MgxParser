#ifndef MGXPARSER_CURSOR_H_
#define MGXPARSER_CURSOR_H_

#define RECSTREAM std::vector<uint8_t>

#include <vector>
#include "tools/searcher.h"
#include "encoding_converter.h"

class RecCursor {
public:
    explicit RecCursor(RECSTREAM &rec_stream)
            : rec_stream_(rec_stream), current_(rec_stream.begin()) {};

    // Go to position
    RecCursor &operator()(std::size_t pos);

    // Go forward/backward by n
    RecCursor &operator+=(long pos);

    RecCursor &operator-=(long pos);

    // Get pointer to current position
    inline uint8_t *Ptr() { return &current_[0]; }

    // Read bytes into variable
    template<typename T>
    RecCursor &operator>>(T &v) {
        auto T_size = sizeof(T);
        if ((*this)() + T_size > rec_stream_.size())
            throw "Cursor go out of range when reading into a variable.";

        memcpy(&v, &current_[0], T_size);
        current_ += T_size;
        return *this;
    }

    RecCursor &operator>>(std::string &s);

    // Alias to &=, useful when chaining
    inline RecCursor &operator>>(long n) {
        *this += n;
        return *this;
    }

    // Tell current offset
    inline std::size_t operator()() { return current_ - rec_stream_.begin(); }

    // Test following bytes
    inline bool Test(void *p, uint32_t len) { return 0 == memcmp(&current_[0], p, len); }

    inline bool TestAndSkip(void *p, uint32_t len) {
        if (Test(p, len)) {
            current_ += len;
            return true;
        } else { return false; }
    }

private:
    RECSTREAM &rec_stream_;
    RECSTREAM::iterator current_;
    std::unique_ptr<EncodingConverter> encoding_converter_;
};

#endif //MGXPARSER_CURSOR_H_
