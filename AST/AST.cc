#include "AST.h"
#include "SymbolTable/Symbol.h"
#include "Tokenizer/Token.h"
#include <string>

AST_Node::AST_Node(Location const location) : location{location} {}

AST_BinaryOperation::AST_BinaryOperation(Location const  location,
                                         AST_Expression *lhs,
                                         AST_Expression *rhs, std::string name,
                                         int precedence)
    : AST_Node(location), lhs{lhs}, rhs{rhs}, name{name}, precedence{precedence}
{}

AST_BinaryOperation::~AST_BinaryOperation()
{
    delete lhs;
    delete rhs;
}

AST_If::AST_If(Location const location, AST_Expression *condition,
               AST_StatementList *body)
    : AST_Node(location), condition{condition}, body{body}
{}

AST_If::~AST_If()
{
    delete condition;
    delete body;
}

AST_Return::AST_Return(Location const      location,
                       AST_ExpressionList *return_values)
    : AST_Node(location), return_values{return_values}
{}

AST_Return::~AST_Return() { delete return_values; }

AST_FunctionDefinition::AST_FunctionDefinition(
    Location const location, AST_Identifier *name,
    AST_ParameterList *parameter_list, AST_ParameterList *return_values,
    AST_StatementList *body)
    : AST_Node(location), name{name}, parameter_list{parameter_list},
      return_values{return_values}, body{body}
{}

AST_FunctionDefinition::~AST_FunctionDefinition()
{
    delete name;
    delete parameter_list;
    delete return_values;
    delete body;
}

AST_VariableDefinition::AST_VariableDefinition(Location const  location,
                                               AST_Identifier *lhs,
                                               AST_Expression *rhs)
    : AST_Node(location), lhs{lhs}, rhs{rhs}
{}

AST_VariableDefinition::~AST_VariableDefinition()
{
    delete lhs;
    delete rhs;
}

AST_VariableAssignment::AST_VariableAssignment(Location const  location,
                                               AST_Identifier *lhs,
                                               AST_Expression *rhs)
    : AST_Node(location), lhs{lhs}, rhs{rhs}
{}

AST_VariableAssignment::~AST_VariableAssignment()
{
    delete lhs;
    delete rhs;
}

AST_ExpressionList::AST_ExpressionList(Location const  location,
                                       AST_Expression *last_expression)
    : AST_Node(location)
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

AST_ParameterList::AST_ParameterList(Location const  location,
                                     AST_Identifier *last_parameter)
    : AST_Node(location)
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

AST_StatementList::AST_StatementList(Location const location,
                                     AST_Statement *last_statement)
    : AST_Node(location)
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

AST_Plus::AST_Plus(Location const location, AST_Expression *lhs,
                   AST_Expression *rhs)
    : AST_Node(location), AST_BinaryOperation(location, lhs, rhs, "Add", 1)
{}

AST_Minus::AST_Minus(Location const location, AST_Expression *lhs,
                     AST_Expression *rhs)
    : AST_Node(location), AST_BinaryOperation(location, lhs, rhs, "Subtract", 1)
{}

AST_Multiplication::AST_Multiplication(Location const  location,
                                       AST_Expression *lhs, AST_Expression *rhs)
    : AST_Node(location), AST_BinaryOperation(location, lhs, rhs, "Multiply", 2)
{}

AST_Division::AST_Division(Location const location, AST_Expression *lhs,
                           AST_Expression *rhs)
    : AST_Node(location), AST_BinaryOperation(location, lhs, rhs, "Divide", 2)
{}

AST_Integer::AST_Integer(Location const location, long value)
    : AST_Node(location), value{value}
{}

AST_Real::AST_Real(Location const location, double value)
    : AST_Node(location), value{value}
{}

AST_UnaryMinus::AST_UnaryMinus(Location const location, AST_Expression *expr)
    : AST_Node(location), expr{expr}
{}

AST_UnaryMinus::~AST_UnaryMinus() { delete expr; }

AST_Identifier::AST_Identifier(Location const location, int symbol_index)
    : AST_Node(location), symbol_index{symbol_index}
{}

AST_FunctionCall::AST_FunctionCall(Location const      location,
                                   AST_Identifier     *ident,
                                   AST_ExpressionList *arguments)
    : AST_Node(location), ident{ident}, arguments{arguments}
{}

AST_FunctionCall::~AST_FunctionCall()
{
    delete ident;
    delete arguments;
}
