#pragma once

#include "Tokenizer/Token.h"

#define MA_DEBUG 1
#define MA_ASM_COM 1

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

void report_internal_compiler_error(std::string const message);

void report_parse_error(Location const &location, std::string const message);
void report_parse_error_unexpected_token(Token const &token);
void report_parse_error_undefined_reference(Token const &token);

void report_type_error(Location const &location, std::string const message);
