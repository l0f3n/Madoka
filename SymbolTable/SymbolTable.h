#pragma once

#include "SymbolTable/Symbol.h"
#include "SymbolTable/SymbolTable.h"
#include <array>
#include <iostream>
#include <string>

class SymbolTable
{
  public:
    SymbolTable();
    ~SymbolTable();

    int insert_variable(Location const &, std::string const &, int type);
    int insert_function(Location const &, std::string const &);
    int insert_type(Location const &, std::string const &, int size);

    int insert_symbol(Location const &, const std::string &name,
                      Symbol::Tag tag);
    // int insert_symbol(const std::string &name, Symbol::Tag tag);

    int     lookup_symbol(const std::string &name) const;
    Symbol *get_symbol(int symbol_index) const;
    Symbol *remove_symbol(int symbol_index);

    void open_scope();
    void close_scope();

    int enclosing_scope() const;

    int generate_temporary_variable(int type);
    int get_next_label();

    void print(std::ostream &os);

  private:
    int hash(const std::string &name) const;

    // TODO: Use vector instead of array and reserve these values to begin with,
    // instead of setting hard upper limit
    static const int MAX_HASH_VALUE = 1024;
    static const int MAX_SYMBOLS    = 1024;
    static const int MAX_LEVELS     = 1024;

    std::array<Symbol *, MAX_SYMBOLS> symbol_table{};
    std::array<int, MAX_LEVELS>       block_table{};
    std::array<int, MAX_HASH_VALUE>   hash_table{};

    int current_symbol_index{-1};
    int current_level{0};
    int current_temporary_variable_number{-1};
    int current_label_number;

    Location no_location{Location{-1, -1, -1}};

    int void_type{-1};
};
