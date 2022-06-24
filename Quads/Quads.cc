#include "Quads.h"
#include "AST/AST.h"
#include "Error/Error.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>

Quad::Quad(Operation operation, long operand1, long operand2, long dest)
    : operation{operation}, operand1{operand1}, operand2{operand2}, dest{dest}
{}

Quads::Quads(SymbolTable *symbol_table) : symbol_table{symbol_table} {}

void Quads::generate_quads(AST_Node *root) { root->generate_quads(this); }

void Quads::add_quad(Quad *quad) { quads.push_back(quad); }

std::ostream &operator<<(std::ostream &os, Quads const &q)
{
    for (Quad const *quad : q.quads)
    {
        os << *quad << std::endl;
    }

    return os;
}

std::ostream &operator<<(std::ostream &os, Quad const &q)
{
    return os << q.operation << "(" << q.operand1 << ", " << q.operand2 << ", "
              << q.dest << ")";
}

std::ostream &operator<<(std::ostream &os, Quad::Operation const &op)
{
    switch (op)
    {
    case Quad::Operation::I_ADD: return os << "I_ADD";
    case Quad::Operation::I_STORE: return os << "I_STORE";
    case Quad::Operation::ASSIGN: return os << "ASSIGN";
    case Quad::Operation::PARAM: return os << "PARAM";
    case Quad::Operation::LABEL: return os << "LABEL";
    case Quad::Operation::FUNCTION_CALL: return os << "FUNCTION_CALL";
    case Quad::Operation::RETURN: return os << "RETURN";
    default: return os << "Unknown operation";
    }
}

// =====================================
// ===== Definition of AST methods =====
// =====================================

int AST_Node::generate_quads(Quads *quads) const
{
    internal_compiler_error()
        << "Call to AST_Node::generate_quads()" << std::endl;
    std::exit(1);
    return -1;
}

int AST_Identifier::generate_quads(Quads *quads) const { return symbol_index; }

int AST_Integer::generate_quads(Quads *quads) const
{
    int type = quads->symbol_table->lookup_symbol("integer");
    int dest = quads->symbol_table->generate_temporary_variable(type);
    quads->add_quad(new Quad{Quad::Operation::I_STORE, value, 0, dest});
    return dest;
}

int AST_Real::generate_quads(Quads *quads) const { return -1; }

int AST_UnaryMinus::generate_quads(Quads *quads) const { return -1; }

int AST_Plus::generate_quads(Quads *quads) const
{
    ASSERT(lhs != nullptr);
    ASSERT(rhs != nullptr);

    int operand1 = lhs->generate_quads(quads);
    int operand2 = rhs->generate_quads(quads);

    // TODO: Figure of what type the resulting computation is going to have
    // and generate a temporary of that type. Right now we alaways use
    // integer

    int type = quads->symbol_table->lookup_symbol("integer");
    int dest = quads->symbol_table->generate_temporary_variable(type);

    quads->add_quad(new Quad{Quad::Operation::I_ADD, operand1, operand2, dest});

    return dest;
}

int AST_Minus::generate_quads(Quads *quads) const { return -1; }

int AST_Multiplication::generate_quads(Quads *quads) const { return -1; }

int AST_Division::generate_quads(Quads *quads) const { return -1; }

int AST_ParameterList::generate_quads(Quads *quads) const { return -1; }

int AST_ExpressionList::generate_quads(Quads *quads) const { return -1; }

int AST_StatementList::generate_quads(Quads *quads) const { return -1; }

int AST_If::generate_quads(Quads *quads) const { return -1; }

int AST_VariableDefinition::generate_quads(Quads *quads) const
{
    quads->add_quad(new Quad(Quad::Operation::ASSIGN,
                             rhs->generate_quads(quads), 0, lhs->symbol_index));

    return -1;
}

int AST_VariableAssignment::generate_quads(Quads *quads) const
{
    quads->add_quad(new Quad(Quad::Operation::ASSIGN,
                             rhs->generate_quads(quads), 0, lhs->symbol_index));

    return -1;
}

int AST_FunctionDefinition::generate_quads(Quads *quads) const
{
    // NOTE: The top of the function, where calls to this function will jump
    long label_number = quads->symbol_table->get_next_label();
    quads->add_quad(
        new Quad(Quad::Operation::LABEL, label_number, name->symbol_index, -1));

    ASSERT(body != nullptr);
    body->generate_quads(quads);

    return -1;
}
int AST_Return::generate_quads(Quads *quads) const
{
    // TODO: Support multiple return values
    /*
    if (return_values != nullptr && return_values->expressions.size() > 1)
    {
        std::cout << "Erorr: Can't return multiple values from function yet"
                  << std::endl;
        std::exit(1);
    }
    */

    int return_value =
        return_values != nullptr ? return_values->generate_quads(quads) : -1;

    // TODO: Normally you would just put your return value in the RAX
    // register but since we want to support multiple return values we need
    // to put them somewhere else and handle it in some other way
    quads->add_quad(new Quad(Quad::Operation::RETURN, return_value, -1, -1));

    return -1;
}

int AST_FunctionCall::generate_quads(Quads *quads) const
{
    // TODO: Use actual type of the return value
    // TODO: Implement multiple return values
    int type         = quads->symbol_table->lookup_symbol("integer");
    int return_value = quads->symbol_table->generate_temporary_variable(type);

    // NOTE: Calculate all arguments and put them on the stack

    if (arguments)
    {
        arguments->generate_quads(quads);
    }

    // TODO: Third parameter to quad should be number of parameters
    quads->add_quad(new Quad(Quad::Operation::FUNCTION_CALL,
                             ident->symbol_index, -1, return_value));

    return return_value;
}
