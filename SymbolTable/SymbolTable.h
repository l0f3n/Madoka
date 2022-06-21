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

    int insert_variable(const std::string &name, int type);
    int insert_function(const std::string &name);
    int insert_type(const std::string &name, int size);

    int     insert_symbol(const std::string &name, Symbol::Tag tag);
    int     lookup_symbol(const std::string &name) const;
    Symbol *get_symbol(int symbol_index) const;
    Symbol *remove_symbol(int symbol_index);

    void open_scope();
    void close_scope();

    int enclosing_scope() const;

    // TODO: This should possibly return int
    Symbol *generate_temporary_variable();
    long    get_next_label();

    void print(std::ostream &os);

  private:
    int hash(const std::string &name) const;

    static const int MAX_HASH_VALUE = 1024;
    static const int MAX_SYMBOLS    = 1024;
    static const int MAX_LEVELS     = 1024;

    std::array<Symbol *, MAX_SYMBOLS> symbol_table;
    std::array<int, MAX_LEVELS>       block_table;
    std::array<int, MAX_HASH_VALUE>   hash_table;

    int current_symbol_index;
    int current_level;

    // long label_number;
};
