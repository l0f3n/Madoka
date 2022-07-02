#pragma once

#include "AST/AST.h"
#include "SymbolTable/Symbol.h"
#include "SymbolTable/SymbolTable.h"

class TypeChecker
{
  public:
    TypeChecker(SymbolTable *const symbol_table);
    void type_check(AST_Node const *const root);

    void type_check_arguments(AST_FunctionCall const *, int,
                              AST_ExpressionList *);

    int type_check_binary_operation(AST_BinaryOperation const *);
    int type_check_binary_relation(AST_BinaryRelation const *);

    SymbolTable *const symbol_table;
};
