#pragma once

#include "SymbolTable/Symbol.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>
#include <string>
#include <vector>

class SymbolTable
{
  public:
    SymbolTable();
    SymbolTable(SymbolTable *prev);

    void    add_symbol(Symbol *symbol);
    Symbol *find_symbol(std::string name);
    Symbol *generate_temporary_variable();

    void print(std::ostream &os);

    // TODO: This is public temporarily when we generate silly code with no ARs
    int offset{0};

  private:
    SymbolTable          *prev;
    std::vector<Symbol *> symbols{};
};
