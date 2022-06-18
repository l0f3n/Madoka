#pragma once

#include <iostream>
#include <string>

class Symbol
{
  public:
    Symbol(std::string name, int type);

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol);

    std::string name;
    int         type;   // Index into the symbol table
    int         offset; // Where in memory this symbol is to be stored
};
