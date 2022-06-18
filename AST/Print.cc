#include "AST.h"
#include <iostream>
#include <sstream>
#include <vector>

void AST_Node::print(std::ostream &os)
{
    std::vector<bool> is_left_history{};
    print(os, true, is_left_history);
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

void AST_Node::print(std::ostream &os, bool is_left,
                     std::vector<bool> is_left_history) const
{
    os << "Warning: Wrong implementation" << std::endl;
}

void AST_BinaryOperation::print(std::ostream &os, bool is_left,
                                std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ " << name << std::endl;

    is_left_history.push_back(is_left);

    lhs->print(os, true, is_left_history);
    rhs->print(os, false, is_left_history);

    is_left_history.pop_back();
}

void AST_Integer::print(std::ostream &os, bool is_left,
                        std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << value << std::endl;
}

void AST_Real::print(std::ostream &os, bool is_left,
                     std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << value << std::endl;
}

void AST_UnaryMinus::print(std::ostream &os, bool is_left,
                           std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << "+ UnaryMinus" << std::endl;

    is_left_history.push_back(is_left);

    expr->print(os, false, is_left_history);

    is_left_history.pop_back();
}

void AST_Identifier::print(std::ostream &os, bool is_left,
                           std::vector<bool> is_left_history) const
{
    os << indent(is_left_history) << name << std::endl;
}

void AST_FunctionCall::print(std::ostream &os, bool is_left,
                             std::vector<bool> is_left_history) const
{
    std::cout << indent(is_left_history) << "FunctionCall" << std::endl;

    is_left_history.push_back(is_left);

    ident->print(os, true, is_left_history);
    expr->print(os, false, is_left_history);

    is_left_history.pop_back();
}
