#include "AST.h"
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
    os << indent(is_left_history) << "+ " << name << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, symbol_table, true, is_left_history);
    rhs->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_Integer::print(std::ostream &os, SymbolTable *symbol_table,
                        bool is_left, std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << value << std::endl;
}

void AST_Real::print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
                     std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << value << std::endl;
}

void AST_UnaryMinus::print(std::ostream &os, SymbolTable *symbol_table,
                           bool              is_left,
                           std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ UnaryMinus" << std::endl;

    is_left_history.push_back(is_left);

    expr->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_Identifier::print(std::ostream &os, SymbolTable *symbol_table,
                           bool              is_left,
                           std::vector<bool> is_left_history) const
{
    // TODO: Print actual name
    os << indent(is_left_history) << "An identifier" << std::endl;
}

void AST_FunctionCall::print(std::ostream &os, SymbolTable *symbol_table,
                             bool              is_left,
                             std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "FunctionCall" << std::endl;

    is_left_history.push_back(is_left);

    ident->print(os, symbol_table, true, is_left_history);
    arguments->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_FunctionDefinition::print(std::ostream &os, SymbolTable *symbol_table,
                                   bool              is_left,
                                   std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ FunctionDefinition" << std::endl;

    is_left_history.push_back(is_left);

    name->print(os, symbol_table, true, is_left_history);

    if (parameter_list)
    {
        parameter_list->print(os, symbol_table, true, is_left_history);
    }

    if (return_values)
    {
        return_values->print(os, symbol_table, true, is_left_history);
    }

    body->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_StatementList::print(std::ostream &os, SymbolTable *symbol_table,
                              bool              is_left,
                              std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ Statements" << std::endl;

    is_left_history.push_back(is_left);

    for (int i = statements.size() - 1; i >= 0; --i)
    {
        statements[i]->print(os, symbol_table, i != 0, is_left_history);
    }

    is_left_history.pop_back();
}

void AST_ParameterList::print(std::ostream &os, SymbolTable *symbol_table,
                              bool              is_left,
                              std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ Parameters" << std::endl;

    is_left_history.push_back(is_left);

    for (int i = parameters.size() - 1; i >= 0; --i)
    {
        parameters[i]->print(os, symbol_table, i != 0, is_left_history);
    }

    is_left_history.pop_back();
}

void AST_ExpressionList::print(std::ostream &os, SymbolTable *symbol_table,
                               bool              is_left,
                               std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ Expressions" << std::endl;

    is_left_history.push_back(is_left);

    for (int i = expressions.size() - 1; i >= 0; --i)
    {
        expressions[i]->print(os, symbol_table, i != 0, is_left_history);
    }

    is_left_history.pop_back();
}

void AST_Return::print(std::ostream &os, SymbolTable *symbol_table,
                       bool is_left, std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ Return" << std::endl;

    is_left_history.push_back(is_left);

    if (return_values)
    {
        return_values->print(os, symbol_table, false, is_left_history);
    }

    is_left_history.pop_back();
}

void AST_If::print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
                   std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "+ If" << std::endl;

    is_left_history.push_back(is_left);

    condition->print(os, symbol_table, true, is_left_history);
    body->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_VariableDefinition::print(std::ostream &os, SymbolTable *symbol_table,
                                   bool              is_left,
                                   std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ Definition" << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, symbol_table, true, is_left_history);
    rhs->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}

void AST_VariableAssignment::print(std::ostream &os, SymbolTable *symbol_table,
                                   bool              is_left,
                                   std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ Assignment" << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, symbol_table, true, is_left_history);
    rhs->print(os, symbol_table, false, is_left_history);

    is_left_history.pop_back();
}
