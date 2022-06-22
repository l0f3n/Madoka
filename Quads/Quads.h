#pragma once

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
        PARAM,
        LABEL,
        FUNCTION_CALL,
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

    std::vector<Quad *>::iterator begin() { return std::begin(quads); };
    std::vector<Quad *>::iterator end() { return std::end(quads); };

    friend std::ostream &operator<<(std::ostream &os, Quads const &q);

    SymbolTable *symbol_table;

  private:
    std::vector<Quad *> quads;
};

std::ostream &operator<<(std::ostream &os, Quad::Operation const &op);
