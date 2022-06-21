#pragma once

#include "Tokenizer/Token.h"

std::ostream &error(Location const &location);
std::ostream &internal_compiler_error();
