#ifndef MGXPARSER_SEARCHER_H_
#define MGXPARSER_SEARCHER_H_

// Until 2022/10, clang on MacOS M1 don't support std::boyer_moore_searcher
// This is a workaround for working on my laptop.
#ifndef __cpp_lib_boyer_moore_searcher
#define SEARCHER std::default_searcher
#else
#define SEARCHER std::boyer_moore_searcher
#endif

#include <algorithm>

template<typename T, typename Tn>
T SearchPattern(T haystackBeg, T haystackEnd, Tn needleBeg, Tn needleEnd) {
    return std::search(
            haystackBeg, haystackEnd,
            SEARCHER(needleBeg, needleEnd));
}

#endif