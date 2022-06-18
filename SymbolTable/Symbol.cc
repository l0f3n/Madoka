#include "Symbol.h"

Symbol::Symbol(std::string name, int type) : name{name}, type{type} {}

std::ostream &operator<<(std::ostream &os, Symbol &symbol)
{
    return os << "Symbol(" << symbol.name << ", " << symbol.type << ")";
}
