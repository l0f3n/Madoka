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

    Quad(Operation, Symbol *, Symbol *, Symbol *);
    Quad(Operation, Symbol *, long, Symbol *);
    Quad(Operation, long, Symbol *, Symbol *);
    Quad(Operation, double, Symbol *, Symbol *);

    Operation operation;
    union
    {
        long    integer_value1;
        double  real_value1;
        Symbol *operand1;
    };

    union
    {
        long    integer_value2;
        double  real_value2;
        Symbol *operand2;
    };

    Symbol *dest;

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
