#include "Quads.h"
#include "AST/AST.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>

Quad::Quad(Operation operation, Symbol *operand1, Symbol *operand2,
           Symbol *dest)
    : operation{operation}, operand1{operand1}, operand2{operand2}, dest{dest}
{}

Quad::Quad(Operation operation, Symbol *operand1, long integer_value2,
           Symbol *dest)
    : operation{operation}, operand1{operand1},
      integer_value2{integer_value2}, dest{dest}
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

Quads::Quads(SymbolTable *symbol_table) : symbol_table{symbol_table} {}

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
        case Quad::Operation::I_ADD:
        case Quad::Operation::ASSIGN:
        case Quad::Operation::PARAM:
        case Quad::Operation::FUNCTION_CALL:
        case Quad::Operation::RETURN:
        {
            os << q.operand1;
            break;
        }
        case Quad::Operation::I_STORE:
        case Quad::Operation::LABEL:
        {
            os << q.integer_value1;
            break;
        }
        default: os << "Unhandled operation"; break;
    }

    os << ", ";

    switch (q.operation)
    {
        case Quad::Operation::I_ADD:
        case Quad::Operation::I_STORE:
        case Quad::Operation::ASSIGN:
        case Quad::Operation::PARAM:
        case Quad::Operation::LABEL:
        case Quad::Operation::RETURN:
        {
            os << q.operand2;
            break;
        }
        case Quad::Operation::FUNCTION_CALL:
        {
            os << q.integer_value2;
            break;
        }
        default: os << "Unhandled operation"; break;
    }

    return os << ", " << q.dest << ")";
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

Symbol *AST_Node::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Identifier::generate_quads(Quads *quads) const
{
    // TODO: Implement this
    return nullptr;
}

Symbol *AST_Integer::generate_quads(Quads *quads) const
{
    // TODO: Implement this
    // Symbol *dest = quads->symbolTable->generate_temporary_variable();
    // quads->add_quad(new Quad{Quad::Operation::I_STORE, value, nullptr,
    // dest}); return dest;
    return nullptr;
}

Symbol *AST_Real::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_UnaryMinus::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Plus::generate_quads(Quads *quads) const
{
    // TODO: Implement this
    // Symbol *operand1 = lhs->generate_quads(quads);
    // Symbol *operand2 = rhs->generate_quads(quads);
    // Symbol *dest     = quads->symbolTable->generate_temporary_variable();
    // quads->add_quad(new Quad{Quad::Operation::I_ADD, operand1, operand2,
    // dest}); return dest;
    return nullptr;
}

Symbol *AST_Minus::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_Multiplication::generate_quads(Quads *quads) const
{
    return nullptr;
}

Symbol *AST_Division::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_ParameterList::generate_quads(Quads *quads) const
{
    return nullptr;
}

Symbol *AST_ExpressionList::generate_quads(Quads *quads) const
{
    Symbol *argument;

    for (auto it{expressions.rbegin()}; it != expressions.rend();)
    {
        argument = (*it++)->generate_quads(quads);

        // NOTE: This function is only used for argument list for function
        // calls at the moment, so this is fine. If it ends up being used for
        // something else later this needs to change
        // TODO: Oops, it is also used for multiple return values. We can't do
        // it like this. Add returnlist/argumentlist or something to distinguish
        // between them?
        quads->add_quad(
            new Quad(Quad::Operation::PARAM, argument, nullptr, nullptr));
    }

    return argument;
}

Symbol *AST_StatementList::generate_quads(Quads *quads) const
{
    for (auto it{statements.rbegin()}; it != statements.rend();)
    {
        Symbol *statement = (*it++)->generate_quads(quads);
    }

    return nullptr;
}

Symbol *AST_If::generate_quads(Quads *quads) const { return nullptr; }

Symbol *AST_VariableDefinition::generate_quads(Quads *quads) const
{
    /*
    if (lhs->symbol->type == "int")
    {
        quads->add_quad(new Quad(Quad::Operation::ASSIGN,
                                 rhs->generate_quads(quads), nullptr,
                                 lhs->symbol));
    }
    */

    return nullptr;
}

Symbol *AST_VariableAssignment::generate_quads(Quads *quads) const
{
    /*
    if (lhs->symbol->type == "int")
    {
        quads->add_quad(new Quad(Quad::Operation::I_LOAD,
                                 rhs->generate_quads(quads), nullptr,
                                 lhs->symbol));
    }
    */
    return nullptr;
}

Symbol *AST_FunctionDefinition::generate_quads(Quads *quads) const
{
    // NOTE: The top of the function, where calls to this function will jump
    // TODO: Implement this
    // long label_number = quads->symbolTable->get_next_label();
    // quads->add_quad(
    // new Quad(Quad::Operation::LABEL, label_number, name->symbol, nullptr));

    // body->generate_quads(quads);

    return nullptr;
}
Symbol *AST_Return::generate_quads(Quads *quads) const
{

    if (return_values->expressions.size() > 1)
    {
        std::cout << "Erorr: Can't return multiple values from function"
                  << std::endl;
        std::exit(1);
    }

    Symbol *return_value = return_values->generate_quads(quads);

    // TODO: Normally you would just put your return value in the RAX register
    // but since we want to support multiple return values we need to put them
    // somewhere else and handle it in some other way
    quads->add_quad(
        new Quad(Quad::Operation::RETURN, return_value, nullptr, nullptr));

    return nullptr;
}

Symbol *AST_FunctionCall::generate_quads(Quads *quads) const
{
    // TODO: Implement this
    // Symbol *return_value = quads->symbolTable->generate_temporary_variable();

    // NOTE: Calculate all arguments and put them on the stack
    // arguments->generate_quads(quads);

    // TODO: Third parameter to quad should be number of parameters
    // quads->add_quad(new Quad(Quad::Operation::FUNCTION_CALL, ident->symbol,
    // nullptr, return_value));

    // return return_value;
    return nullptr;
}
