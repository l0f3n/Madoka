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

    Location const    location{Location{-1, -1, -1, -1}};
    std::string const name{""};
    Tag               tag{Symbol::Tag::Undefined};
    int               type{-1};
    int               level{-1};
    int               offset{-1};
    int               hash_link{-1};

    friend std::ostream &operator<<(std::ostream &os, Symbol &symbol);
};

std::ostream &operator<<(std::ostream &os, Symbol::Tag &tag);

class VariableSymbol : public Symbol
{
  public:
    VariableSymbol(Location const &location, const std::string &name);
};

class FunctionSymbol : public Symbol
{
  public:
    FunctionSymbol(Location const &location, const std::string &name);

    int first_parameter{-1};
    int label{-1};
    int activation_record_size{0};
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
