#include "AST.h"
#include "Tokenizer/Token.h"
#include <Error/Error.h>
#include <iostream>
#include <sstream>
#include <vector>

void AST_Node::print(std::ostream &os, SymbolTable *symbol_table)
{
    std::vector<bool> is_left_history{};
    print(os, symbol_table, true, is_left_history);
}

std::string AST_Node::indent(std::vector<bool> is_left_history) const
{
    std::ostringstream oss{};
    for (int i = 1; i < is_left_history.size(); i++)
    {
        oss << (is_left_history[i] ? "|    " : "     ");
    }

    oss << (is_left_history.size() > 0 ? "| -- " : "");

    return oss.str();
}

void AST_Node::print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
                     std::vector<bool> is_left_history) const
{
    os << "Warning: Wrong implementation" << std::endl;
}

void AST_BinaryOperation::print(std::ostream &os, SymbolTable *symbol_table,
                                bool              is_left,
                                std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ " << name << " (" << location << ")"
       << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, symbol_table, true, is_left_history);
    rhs->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_Integer::print(std::ostream &os, SymbolTable *symbol_table,
                        bool is_left, std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << value << " (" << location << ")"
       << std::endl;
}

void AST_Real::print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
                     std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << value << " (" << location << ")"
       << std::endl;
}

void AST_UnaryMinus::print(std::ostream &os, SymbolTable *symbol_table,
                           bool              is_left,
                           std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ UnaryMinus"
       << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    expr->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_Identifier::print(std::ostream &os, SymbolTable *symbol_table,
                           bool              is_left,
                           std::vector<bool> is_left_history) const
{
    Symbol *symbol = symbol_table->get_symbol(symbol_index);
    os << indent(is_left_history) << symbol->name << " (" << location << ")"
       << std::endl;
}

void AST_FunctionCall::print(std::ostream &os, SymbolTable *symbol_table,
                             bool              is_left,
                             std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "FunctionCall"
              << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    ident->print(os, symbol_table, true, is_left_history);

    if (arguments)
    {
        arguments->print(os, symbol_table, false, is_left_history);
    }

    is_left_history.pop_back();
}

void AST_FunctionDefinition::print(std::ostream &os, SymbolTable *symbol_table,
                                   bool              is_left,
                                   std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ FunctionDefinition"
              << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    name->print(os, symbol_table, true, is_left_history);

    if (parameter_list)
    {
        parameter_list->print(os, symbol_table, true, is_left_history);
    }

    if (return_type)
    {
        return_type->print(os, symbol_table, true, is_left_history);
    }

    body->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_StatementList::print(std::ostream &os, SymbolTable *symbol_table,
                              bool              is_left,
                              std::vector<bool> is_left_history) const
{

    // NOTE: We cheat a bit here when we print the statements. Is left is always
    // going to be true the first time, so we use it print parameters and add
    // history, but then we set the rest to false not make them not print
    // parameters
    if (is_left)
    {

        std::cout << indent(is_left_history) << "+ Statements"
                  << " (" << location << ")" << std::endl;

        is_left_history.push_back(is_left);
    }

    ASSERT(statement != nullptr);
    statement->print(os, symbol_table, false, is_left_history);

    if (rest_statements)
    {
        rest_statements->print(os, symbol_table, false, is_left_history);
    }

    if (is_left)
    {
        is_left_history.pop_back();
    }
}

void AST_ParameterList::print(std::ostream &os, SymbolTable *symbol_table,
                              bool              is_left,
                              std::vector<bool> is_left_history) const
{
    // NOTE: We cheat a bit here when we print the parameters. Is left is always
    // going to be true the first time, so we use it print parameters and add
    // history, but then we set the rest to false not make them not print
    // parameters
    if (is_left)
    {

        std::cout << indent(is_left_history) << "+ Parameters"
                  << " (" << location << ")" << std::endl;

        is_left_history.push_back(is_left);
    }

    ASSERT(parameter != nullptr);
    parameter->print(os, symbol_table, false, is_left_history);

    if (rest_parameters)
    {
        rest_parameters->print(os, symbol_table, false, is_left_history);
    }

    if (is_left)
    {
        is_left_history.pop_back();
    }
}

void AST_ExpressionList::print(std::ostream &os, SymbolTable *symbol_table,
                               bool              is_left,
                               std::vector<bool> is_left_history) const
{
    // NOTE: We cheat a bit here when we print the parameters. Is left is always
    // going to be true the first time, so we use it print parameters and add
    // history, but then we set the rest to false not make them not print
    // parameters
    if (is_left)
    {

        std::cout << indent(is_left_history) << "+ Expressions"
                  << " (" << location << ")" << std::endl;

        is_left_history.push_back(is_left);
    }

    ASSERT(expression != nullptr);
    expression->print(os, symbol_table, false, is_left_history);

    if (rest_expressions)
    {
        rest_expressions->print(os, symbol_table, false, is_left_history);
    }

    if (is_left)
    {
        is_left_history.pop_back();
    }
}

void AST_Return::print(std::ostream &os, SymbolTable *symbol_table,
                       bool is_left, std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ Return"
       << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    if (expression)
    {
        expression->print(os, symbol_table, false, is_left_history);
    }

    is_left_history.pop_back();
}

void AST_If::print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
                   std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ If"
              << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    condition->print(os, symbol_table, true, is_left_history);
    body->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_VariableDefinition::print(std::ostream &os, SymbolTable *symbol_table,
                                   bool              is_left,
                                   std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ Definition"
       << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, symbol_table, true, is_left_history);
    rhs->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_VariableAssignment::print(std::ostream &os, SymbolTable *symbol_table,
                                   bool              is_left,
                                   std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ Assignment"
       << " (" << location << ")" << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, symbol_table, true, is_left_history);
    rhs->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}
