#include "Symbol.h"

Symbol::~Symbol() {}

Symbol::Symbol(const std::string &name)
    : name{name}, tag{Symbol::Tag::Undefined}, level{-1}, hash_link{-1}
{}

VariableSymbol::VariableSymbol(const std::string &name)
    : Symbol(name), offset{-1}
{}

FunctionSymbol::FunctionSymbol(const std::string &name)
    : Symbol(name), label{-1}, activation_record_size{0}
{}

TypeSymbol::TypeSymbol(const std::string &name) : Symbol(name) {}

std::ostream &operator<<(std::ostream &os, Symbol &symbol)
{
    return os << "Symbol(" << symbol.name << ", " << symbol.tag << ", "
              << symbol.type << ", " << symbol.level << ", " << symbol.hash_link
              << ")";
}
