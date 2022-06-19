#include "SymbolTable.h"
#include <string>

SymbolTable::SymbolTable() : prev{nullptr} {}

SymbolTable::SymbolTable(SymbolTable *prev) : prev{prev} {}

void SymbolTable::add_symbol(Symbol *symbol)
{
    symbol->offset = offset;
    offset += 8; // Size of the symbol, everything is 8
    symbols.push_back(symbol);
}

Symbol *SymbolTable::find_symbol(std::string name)
{
    for (SymbolTable *symbolTable{this}; symbolTable != nullptr;
         symbolTable = symbolTable->prev)
    {
        for (Symbol *symbol : symbolTable->symbols)
        {
            if (symbol->name == name)
            {
                return symbol;
            }
        }
    }

    return nullptr;
}

Symbol *SymbolTable::generate_temporary_variable()
{
    Symbol *temp = new Symbol(std::to_string(1), "Unknown");
    add_symbol(temp);
    return temp;
}

void SymbolTable::print(std::ostream &os)
{
    os << "Size: " << symbols.size() << std::endl;
    for (Symbol *symbol : symbols)
    {
        os << *symbol << std::endl;
    }
}
