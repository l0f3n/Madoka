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
        I_LOAD,
        I_ADD,
    };

    Quad(Operation, Symbol *, Symbol *, Symbol *);
    Quad(Operation, long, Symbol *, Symbol *);
    Quad(Operation, double, Symbol *, Symbol *);

    Operation operation;
    union
    {
        long    integer_value1;
        double  real_value1;
        Symbol *operand1;
    };

    Symbol *operand2;
    Symbol *dest;

    friend std::ostream &operator<<(std::ostream &os, Quad const &q);
};

class Quads
{
  public:
    Quads(SymbolTable *symbolTable);

    void generate_quads(AST_Node *root);

    void add_quad(Quad *quad);

    std::vector<Quad *>::iterator begin() { return std::begin(quads); };
    std::vector<Quad *>::iterator end() { return std::end(quads); };

    friend std::ostream &operator<<(std::ostream &os, Quads const &q);

    SymbolTable *symbolTable;

  private:
    std::vector<Quad *> quads;
};

std::ostream &operator<<(std::ostream &os, Quad::Operation const &op);
