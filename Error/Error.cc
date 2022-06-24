#include "Error.h"

std::ostream &error(Location const &location)
{
    return std::cout << "SyntaxError:" << location << ": ";
}

void report_parse_error(Location const &location, std::string const message)
{
    std::cout << "ParseError:" << location << ": " << message << std::endl;
    std::exit(1);
}

void report_parse_error_unexpected_token(Token const &token)
{
    report_parse_error(token.location,
                       "Unexpected token: '" + token.text + "'");
}

void report_parse_error_undefined_reference(Token const &token)
{
    report_parse_error(token.location,
                       "Reference to undefined symbol: '" + token.text + "'");
}

void report_type_error(Location const &location, std::string const message)
{
    std::cout << "TypeError:" << location << ": " << message << std::endl;
    std::exit(1);
}

std::ostream &type_error(Location const &location)
{
    return std::cout << "TypeError:" << location << ": ";
}

std::ostream &internal_compiler_error()
{
    return std::cout << "InternalCompilerError: ";
}
