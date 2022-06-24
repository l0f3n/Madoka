#include "Symbol.h"
#include "Tokenizer/Token.h"

Symbol::~Symbol() {}

Symbol::Symbol(Location const &location, const std::string &name)
    : location{location}, name{name}
{}

TypeSymbol::TypeSymbol(Location const &location, std::string const &name)
    : Symbol(location, name)
{}

VariableSymbol::VariableSymbol(Location const    &location,
                               const std::string &name)
    : Symbol(location, name)
{}

FunctionSymbol::FunctionSymbol(Location const    &location,
                               const std::string &name)
    : Symbol(location, name)
{}

ParameterSymbol::ParameterSymbol(Location const    &location,
                                 std::string const &name)
    : Symbol(location, name)
{}

std::ostream &operator<<(std::ostream &os, Symbol &symbol)
{
    return os << "Symbol((" << symbol.location.l1 << "," << symbol.location.c1
              << "), " << symbol.name << ", " << symbol.tag << ", "
              << symbol.level << ")";
}

std::ostream &operator<<(std::ostream &os, Symbol::Tag &tag)
{
    switch (tag)
    {
    case Symbol::Undefined: return os << "Undefined";
    case Symbol::Type: return os << "Type";
    case Symbol::Variable: return os << "Variable";
    case Symbol::Function: return os << "Function";
    case Symbol::Parameter: return os << "Parameter";
    default: return os << "Unknown tag";
    }
}
