#include "Quads.h"
#include "AST/AST.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>

Quad::Quad(Operation operation, Symbol *operand1, Symbol *operand2,
           Symbol *dest)
    : operation{operation}, operand1{operand1}, operand2{operand2}, dest{dest}
{}

Quad::Quad(Operation operation, long integer_value1, Symbol *operand2,
           Symbol *dest)
    : operation{operation},
      integer_value1{integer_value1}, operand2{operand2}, dest{dest}
{}

Quad::Quad(Operation operation, double real_value1, Symbol *operand2,
           Symbol *dest)
    : operation{operation},
      real_value1{real_value1}, operand2{operand2}, dest{dest}
{}

Quads::Quads(SymbolTable *symbolTable) : symbolTable{symbolTable} {}

void Quads::generate_quads(AST_Node *root) { root->generate_quads(this); }

void Quads::add_quad(Quad *quad) { quads.push_back(quad); }

std::ostream &operator<<(std::ostream &os, Quads const &q)
{
    for (Quad *quad : q.quads)
    {
        os << *quad << std::endl;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, Quad const &q)
{
    os << q.operation << "(";
    switch (q.operation)
    {
    case Quad::Operation::I_LOAD: os << q.integer_value1; break;
    case Quad::Operation::I_ADD: os << q.operand1; break;
    default: os << "Unhandled operation"; break;
    }
    return os << ", " << q.operand2 << ", " << q.dest << ")";
}

std::ostream &operator<<(std::ostream &os, Quad::Operation const &op)
{
    switch (op)
    {
    case Quad::Operation::I_ADD: return os << "I_ADD";
    case Quad::Operation::I_LOAD: return os << "I_LOAD";
    default: return os << "Unknown operation";
    }
}

Symbol *AST_Node::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Identifier::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_FunctionCall::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Integer::generate_quads(Quads *quads) const
{
    Symbol *dest = quads->symbolTable->generate_temporary_variable();
    quads->add_quad(new Quad{Quad::Operation::I_LOAD, value, nullptr, dest});
    return dest;
}

Symbol *AST_Real::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_UnaryMinus::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Plus::generate_quads(Quads *quads) const
{
    Symbol *operand1 = lhs->generate_quads(quads);
    Symbol *operand2 = rhs->generate_quads(quads);
    Symbol *dest     = quads->symbolTable->generate_temporary_variable();
    quads->add_quad(new Quad{Quad::Operation::I_ADD, operand1, operand2, dest});
    return dest;
}

Symbol *AST_Minus::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Multiplication::generate_quads(Quads *quads) const
{
    return nullptr;
}

Symbol *AST_Division::generate_quads(Quads *quads) const { return nullptr; }
