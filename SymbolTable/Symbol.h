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

        Type,
        Variable,
        Function,
        Parameter,
    };

    Symbol(Location const &location, const std::string &name);
    virtual ~Symbol();

    Location    location{Location{-1, -1, -1, -1}};
    std::string name{""};
    Tag         tag{Symbol::Tag::Undefined};
    int         type{0}; // We add void first so 0 means void
    int         level{-1};
    int         hash_link{-1};

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol);
};

std::ostream &operator<<(std::ostream &os, Symbol::Tag &tag);

class VariableSymbol : public Symbol
{
  public:
    VariableSymbol(Location const &location, const std::string &name);

    int offset{-1};
};

class FunctionSymbol : public Symbol
{
  public:
    FunctionSymbol(Location const &location, const std::string &name);

    int first_parameter{-1};
    int parameter_count{0};

    int label{-1};
    int activation_record_size{0};

    bool has_return{false};
};

class ParameterSymbol : public Symbol
{
  public:
    ParameterSymbol(Location const &location, const std::string &name);

    int index{-1};
    int next_parameter{-1};
};

class TypeSymbol : public Symbol
{
  public:
    TypeSymbol(Location const &location, const std::string &name);

    int size{-1}; // In bytes
};
