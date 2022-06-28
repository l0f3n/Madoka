#pragma once

#include "SymbolTable/Symbol.h"
#include "SymbolTable/SymbolTable.h"
#include <AST/AST.h>
#include <iostream>
#include <vector>

struct Quad
{
  public:
    enum class Operation
    {
        // Integer operations
        I_STORE,
        I_ADD,

        // Operations that work on both
        ASSIGN,
        FUNCTION_CALL,
        LABEL,
        ARGUMENT,
        RETURN,
    };

    Quad(Operation, long, long, long);

    // NOTE: Integer values, doubles and symbol table indices are stored as long
    Operation operation;
    long      operand1;
    long      operand2;
    long      dest;

    friend std::ostream &operator<<(std::ostream &os, Quad const &q);
};

class Quads
{
  public:
    Quads(SymbolTable *symbol_table);

    void generate_quads(AST_Node *root);
    void add_quad(Quad *quad);

    Quad *get_current_quad();

    void generate_argument_quads(AST_ExpressionList *arguments, int index);

    friend std::ostream &operator<<(std::ostream &os, Quads const &q);

    SymbolTable *symbol_table;

  private:
    int current_quad_index{-1};

    std::vector<Quad *> quads;
};

std::ostream &operator<<(std::ostream &os, Quad::Operation const &op);
