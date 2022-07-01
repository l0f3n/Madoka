#pragma once

#include "AST/AST.h"
#include "Quads/Quads.h"
#include "SymbolTable/Symbol.h"
#include <string>
#include <vector>

class CodeGenerator
{
  public:
    CodeGenerator(std::ostream &out, SymbolTable *symbol_table);

    void generate_code(Quads &quads);

    void generate_function_prologue(FunctionSymbol *function) const;
    void generate_function_epilogue(FunctionSymbol *function) const;

    void generate_entry_code() const;

    void generate_predefined_functions() const;

    std::string get_argument_register(int) const;
    void        store_parameter(int) const;

  private:
    void operation(std::string const) const;
    void label(std::string const) const;
    void label(FunctionSymbol const *function) const;

    std::string address(int symbol_index) const;

    // TODO: There should probably be some sort of register type instead of
    // just a string
    void load(std::string reg, int symbol_index) const;
    void store(int symbol_index, std::string reg) const;

    std::ostream &out;

    SymbolTable *symbol_table;
};
