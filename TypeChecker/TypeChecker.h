#pragma once

#include "AST/AST.h"
#include "SymbolTable/SymbolTable.h"

class TypeChecker
{
  public:
    TypeChecker(SymbolTable *const symbol_table);
    void type_check(AST_Node const *const root);

  private:
    SymbolTable *const symbol_table;
};
