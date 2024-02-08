/***************************************************************
 * \file       searcher.h
 * \author     PATRICK LI (admin@aocrec.com)
 * \date       2022/11/8
 * \copyright  Copyright (c) 2020-2024
 ***************************************************************/

#ifndef MGXPARSER_SEARCHER_H_
#define MGXPARSER_SEARCHER_H_

// Until 2022/10, clang on MacOS M1 don't support std::boyer_moore_searcher
// This is a workaround for working on my laptop.
#if USE_BM_SEARCH
#define SEARCHER std::boyer_moore_searcher
#else
#define SEARCHER std::default_searcher
#endif

#include <algorithm>
#include <functional>

template<typename T, typename Tn>
T SearchPattern(T haystack_begin, T haystack_end, Tn needle_begin, Tn needle_end) {
    return std::search(
            haystack_begin, haystack_end,
            SEARCHER(needle_begin, needle_end));
}

#endif //MGXPARSER_SEARCHER_H_