#pragma once

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

    Symbol(const std::string &name);
    virtual ~Symbol();

    // TODO: Position information struct !!!!!!!!!!!!!!!
    std::string name;
    Tag         tag;
    int         type;
    int         level;
    int         hash_link;

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol);
};

class VariableSymbol : public Symbol
{
  public:
    VariableSymbol(const std::string &name);

    int offset;
};

class FunctionSymbol : public Symbol
{
  public:
    FunctionSymbol(const std::string &name);

    int label;
    int activation_record_size;
};

class TypeSymbol : public Symbol
{
  public:
    TypeSymbol(const std::string &name);

    int size; // In bytes
};
