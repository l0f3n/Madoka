#include "Error.h"

std::ostream &error(Location const &location)
{
    return std::cout << "SyntaxError:" << location << ": ";
}

std::ostream &internal_compiler_error()
{
    return std::cout << "InternalCompilerError: ";
}
