#pragma once

#include "Tokenizer/Token.h"

#define MA_DEBUG 1

#if MA_DEBUG == 1
#define ASSERT(condition)                                                      \
    if (!(condition))                                                          \
    {                                                                          \
        std::cout << __FILE__ << ":" << __LINE__ << ": "                       \
                  << "Assertion failed: " << #condition << std::endl;          \
        std::exit(1);                                                          \
    }
#else
#define ASSERT(condition)
#endif

std::ostream &error(Location const &location);
std::ostream &internal_compiler_error();
