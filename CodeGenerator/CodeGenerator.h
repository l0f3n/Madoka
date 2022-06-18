#pragma once

#include "AST/AST.h"
#include "Quads/Quads.h"
#include "SymbolTable/Symbol.h"
#include <string>

class CodeGenerator
{
  public:
    CodeGenerator(SymbolTable *symbolTable);
    void generate_code(Quads *quads, std::ostream &os) const;

  private:
    // TODO: There should probably be some sort of register type instead of just
    // a string
    void load(std::ostream &os, std::string reg, Symbol *symbol) const;
    void store(std::ostream &os, Symbol *symbol, std::string reg) const;

    void generate_entry_code(std::ostream &os) const;
    void generate_exit_code(std::ostream &os) const;

    SymbolTable *symbolTable;
};
