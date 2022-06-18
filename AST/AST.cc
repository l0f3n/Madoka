#include "AST.h"
#include "SymbolTable/Symbol.h"
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

AST_Integer::AST_Integer(long value) : value{value} {}

AST_Real::AST_Real(double value) : value{value} {}

AST_UnaryMinus::AST_UnaryMinus(AST_Expression *expr) : expr{expr} {}

AST_UnaryMinus::~AST_UnaryMinus() { delete expr; }

AST_Identifier::AST_Identifier(Symbol *symbol, std::string name)
    : symbol{symbol}, name{name}
{}

AST_FunctionCall::AST_FunctionCall(Symbol *symbol, AST_Identifier *ident,
                                   AST_Expression *expr)
    : symbol{symbol}, ident{ident}, expr{expr}
{}

AST_FunctionCall::~AST_FunctionCall()
{
    delete ident;
    delete expr;
}
