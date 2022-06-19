#pragma once

#include <iostream>
#include <string>

class Symbol
{
  public:
    Symbol(std::string name, std::string type);

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol);

    std::string name;
    std::string type;   // TODO: This should be something else later
    int         offset; // Where in memory this symbol is to be stored
};
