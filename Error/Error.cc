#include "Error.h"

void report_internal_compiler_error(const std::string message)
{
    std::cout << "InternalCompilerError: " << message << std::endl;
    std::exit(1);
}

void report_parse_error(Location const &location, std::string const message)
{
    std::cout << "ParseError:";

    if (location.l1 != -1)
    {
        std::cout << location << ":";
    }

    std::cout << " " << message << std::endl;

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
