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

AST_If::AST_If(AST_Expression *condition, AST_StatementList *body)
    : condition{condition}, body{body}
{}

AST_If::~AST_If()
{
    delete condition;
    delete body;
}

AST_Return::AST_Return(AST_ExpressionList *return_values)
    : return_values{return_values}
{}

AST_Return::~AST_Return() { delete return_values; }

AST_FunctionDefinition::AST_FunctionDefinition(
    AST_Identifier *name, AST_ParameterList *parameter_list,
    AST_ParameterList *return_values, AST_StatementList *body)
    : name{name}, parameter_list{parameter_list},
      return_values{return_values}, body{body}
{}

AST_FunctionDefinition::~AST_FunctionDefinition()
{
    delete name;
    delete parameter_list;
    delete return_values;
    delete body;
}

AST_VariableDefinition::AST_VariableDefinition(AST_Identifier *lhs,
                                               AST_Expression *rhs)
    : lhs{lhs}, rhs{rhs}
{}

AST_VariableDefinition::~AST_VariableDefinition()
{
    delete lhs;
    delete rhs;
}

AST_VariableAssignment::AST_VariableAssignment(AST_Identifier *lhs,
                                               AST_Expression *rhs)
    : lhs{lhs}, rhs{rhs}
{}

AST_VariableAssignment::~AST_VariableAssignment()
{
    delete lhs;
    delete rhs;
}

AST_ExpressionList::AST_ExpressionList(AST_Expression *last_expression)
{
    add_expression(last_expression);
}

AST_ExpressionList::~AST_ExpressionList()
{
    for (AST_Expression *expression : expressions)
    {
        delete expression;
    }
}

void AST_ExpressionList::add_expression(AST_Expression *parameter)
{
    expressions.push_back(parameter);
}

AST_ParameterList::AST_ParameterList(AST_Identifier *last_parameter)
{
    add_parameter(last_parameter);
}

AST_ParameterList::~AST_ParameterList()
{
    for (AST_Identifier *parameter : parameters)
    {
        delete parameter;
    }
}

void AST_ParameterList::add_parameter(AST_Identifier *parameter)
{
    parameters.push_back(parameter);
}

AST_StatementList::AST_StatementList(AST_Statement *last_statement)
{
    add_statement(last_statement);
}

AST_StatementList::~AST_StatementList()
{
    for (AST_Statement *statement : statements)
    {
        delete statement;
    }
}

void AST_StatementList::add_statement(AST_Statement *statement)
{
    statements.push_back(statement);
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

AST_FunctionCall::AST_FunctionCall(AST_Identifier     *ident,
                                   AST_ExpressionList *arguments)
    : ident{ident}, arguments{arguments}
{}

AST_FunctionCall::~AST_FunctionCall()
{
    delete ident;
    delete arguments;
}
