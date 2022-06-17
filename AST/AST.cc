#include "AST.h"
#include <string>

AST_BinaryOperation::AST_BinaryOperation(AST_Expression *lhs,
                                         AST_Expression *rhs, std::string name,
                                         int precedence)
    : lhs{lhs}, rhs{rhs}, name{name}, precedence{precedence}
{}

AST_BinaryOperation::~AST_BinaryOperation()
{
    delete lhs;
    delete rhs;
}

AST_Plus::AST_Plus(AST_Expression *lhs, AST_Expression *rhs)
    : AST_BinaryOperation(lhs, rhs, "Add", 1)
{}

AST_Minus::AST_Minus(AST_Expression *lhs, AST_Expression *rhs)
    : AST_BinaryOperation(lhs, rhs, "Subtract", 1)
{}

AST_Multiplication::AST_Multiplication(AST_Expression *lhs, AST_Expression *rhs)
    : AST_BinaryOperation(lhs, rhs, "Multiply", 2)
{}

AST_Division::AST_Division(AST_Expression *lhs, AST_Expression *rhs)
    : AST_BinaryOperation(lhs, rhs, "Divide", 2)
{}

AST_Exponentiation::AST_Exponentiation(AST_Expression *lhs, AST_Expression *rhs)
    : AST_BinaryOperation(lhs, rhs, "Power", 3)
{}

AST_Number::AST_Number(double value) : value{value} {}

AST_UnaryMinus::AST_UnaryMinus(AST_Expression *expr) : expr{expr} {}

AST_UnaryMinus::~AST_UnaryMinus() { delete expr; }

AST_Identifier::AST_Identifier(std::string name) : name{name} {}

AST_FunctionCall::AST_FunctionCall(AST_Identifier *ident, AST_Expression *expr)
    : ident{ident}, expr{expr}
{}

AST_FunctionCall::~AST_FunctionCall()
{
    delete ident;
    delete expr;
}
