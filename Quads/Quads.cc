#include "Quads.h"
#include "AST/AST.h"
#include "Error/Error.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>
#include <string>

Quad::Quad(Operation operation, long operand1, long operand2, long dest)
    : operation{operation}, operand1{operand1}, operand2{operand2}, dest{dest}
{}

Quads::Quads(SymbolTable *symbol_table) : symbol_table{symbol_table} {}

void Quads::generate_quads(AST_Node *root) { root->generate_quads(this); }

void Quads::add_quad(Quad *quad) { quads.push_back(quad); }

Quad *Quads::get_current_quad()
{
    if ((current_quad_index + 1) < quads.size())
    {
        return quads[++current_quad_index];
    }
    else
    {
        return nullptr;
    }
}

void Quads::generate_argument_quads(AST_ExpressionList *arguments, int index)
{
    if (arguments == nullptr)
    {
        return;
    }

    int argument_location = arguments->expression->generate_quads(this);

    add_quad(new Quad(Quad::Operation::ARGUMENT, argument_location, index, -1));

    generate_argument_quads(arguments->rest_expressions, ++index);
}

std::ostream &operator<<(std::ostream &os, Quads const &q)
{
    for (int i = 0; i < q.quads.size(); i++)
    {
        os << *q.quads[i] << std::endl;
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
    case Quad::Operation::I_ADD: return os << "add";
    case Quad::Operation::I_STORE: return os << "store";
    case Quad::Operation::ASSIGN: return os << "assign";
    case Quad::Operation::ARGUMENT: return os << "argument";
    case Quad::Operation::LABEL: return os << "label";
    case Quad::Operation::FUNCTION_CALL: return os << "function call";
    case Quad::Operation::RETURN: return os << "return";
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
    int type = quads->symbol_table->type_integer;
    int dest = quads->symbol_table->generate_temporary_variable(type);
    quads->add_quad(new Quad{Quad::Operation::I_STORE, value, -1, dest});
    return dest;
}

int AST_Real::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_Real::") + __func__ +
                                   "(): no implemented");
    return -1;
}

int AST_UnaryMinus::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_UnaryMinus::") + __func__ +
                                   "(): no implemented");
    return -1;
}

int AST_Plus::generate_quads(Quads *quads) const
{
    ASSERT(lhs != nullptr);
    ASSERT(rhs != nullptr);

    int operand1 = lhs->generate_quads(quads);
    int operand2 = rhs->generate_quads(quads);

    Symbol *operand1_symbol = quads->symbol_table->get_symbol(operand1);
    ASSERT(operand1_symbol != nullptr);

    // NOTE: They have already been type checked and are the same type, right
    // here we just need to figure out which one
    if (operand1_symbol->type == quads->symbol_table->type_integer)
    {
        int type = quads->symbol_table->type_integer;
        int dest = quads->symbol_table->generate_temporary_variable(type);
        quads->add_quad(
            new Quad{Quad::Operation::I_ADD, operand1, operand2, dest});
        return dest;
    }
    else
    {
        report_internal_compiler_error(
            "Addition not implemented for type 'real'");
        return -1;
    }
}

int AST_Minus::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_Minus::") + __func__ +
                                   "(): no implemented");
    return -1;
}

int AST_Multiplication::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_Multiplication::") +
                                   __func__ + "(): no implemented");
    return -1;
}

int AST_Division::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_Division::") + __func__ +
                                   "(): no implemented");
    return -1;
}

int AST_ParameterList::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_ParameterList::") +
                                   __func__ + "(): no implemented");
    return -1;
}

int AST_ExpressionList::generate_quads(Quads *quads) const
{
    report_internal_compiler_error(std::string("AST_ExpressionList::") +
                                   __func__ + "(): no implemented");
    return -1;
}

int AST_StatementList::generate_quads(Quads *quads) const
{
    ASSERT(statement != nullptr);
    statement->generate_quads(quads);

    if (rest_statements)
    {
        rest_statements->generate_quads(quads);
    }

    return -1;
}

int AST_If::generate_quads(Quads *quads) const { return -1; }

int AST_VariableDefinition::generate_quads(Quads *quads) const
{
    // NOTE: At this stage, variable definition and assignment are the same
    quads->add_quad(new Quad(Quad::Operation::ASSIGN,
                             rhs->generate_quads(quads), -1,
                             lhs->symbol_index));

    return -1;
}

int AST_VariableAssignment::generate_quads(Quads *quads) const
{
    quads->add_quad(new Quad(Quad::Operation::ASSIGN,
                             rhs->generate_quads(quads), -1,
                             lhs->symbol_index));

    return -1;
}

int AST_FunctionDefinition::generate_quads(Quads *quads) const
{
    // NOTE: We automatically generate function entry and function exit code
    // in the code generator when we start generating functions. Thats why we
    // basically don't do anything here, except generating the actually body

    ASSERT(body != nullptr);
    body->generate_quads(quads);

    return -1;
}
int AST_Return::generate_quads(Quads *quads) const
{
    int address;
    if (expression != nullptr)
    {
        address = expression->generate_quads(quads);
    }
    else
    {
        address = -1;
    }

    quads->add_quad(new Quad(Quad::Operation::RETURN, address, -1, -1));

    return address;
}

int AST_FunctionCall::generate_quads(Quads *quads) const
{
    // NOTE: Calculate all arguments and put them on the stack
    quads->generate_argument_quads(arguments, 0);

    // TODO: Use actual type of the return value
    // TODO: Implement multiple return values
    int type    = quads->symbol_table->type_integer;
    int address = quads->symbol_table->generate_temporary_variable(type);

    quads->add_quad(new Quad(Quad::Operation::FUNCTION_CALL,
                             ident->symbol_index, -1, address));

    return address;
}
