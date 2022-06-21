#pragma once

#include <Tokenizer/Token.h>
#include <iostream>
#include <string>

class Symbol
{
  public:
    enum Tag
    {
        Undefined,

        Variable,
        Function,
        Type,
    };

    Symbol(Location const &location, const std::string &name);
    virtual ~Symbol();

    Location const    location;
    std::string const name;
    Tag               tag;
    int               type;
    int               level;
    int               hash_link;

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol);
};

std::ostream &operator<<(std::ostream &os, Symbol::Tag &tag);

class VariableSymbol : public Symbol
{
  public:
    VariableSymbol(Location const &location, const std::string &name);

    int offset;
};

class FunctionSymbol : public Symbol
{
  public:
    FunctionSymbol(Location const &location, const std::string &name);

    int label;
    int activation_record_size;
};

class TypeSymbol : public Symbol
{
  public:
    TypeSymbol(Location const &location, const std::string &name);

    int size; // In bytes
};
