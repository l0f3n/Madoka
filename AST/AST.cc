#include "AST.h"
#include "Error/Error.h"
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
    ASSERT(lhs != nullptr);
    ASSERT(rhs != nullptr);

    delete lhs;
    delete rhs;
}

AST_If::AST_If(Location const location, AST_Expression *condition,
               AST_StatementList *body)
    : AST_Node(location), condition{condition}, body{body}
{}

AST_If::~AST_If()
{
    ASSERT(condition != nullptr);
    ASSERT(body != nullptr);

    delete condition;
    delete body;
}

AST_Return::AST_Return(Location const location, AST_Expression *expression)
    : AST_Node(location), expression{expression}
{}

AST_Return::~AST_Return()
{
    if (expression)
    {
        delete expression;
    }
}

AST_FunctionDefinition::AST_FunctionDefinition(
    Location const location, AST_Identifier *name,
    AST_ParameterList *parameter_list, AST_Identifier *return_type,
    AST_StatementList *body)
    : AST_Node(location), name{name}, parameter_list{parameter_list},
      return_type{return_type}, body{body}
{}

AST_FunctionDefinition::~AST_FunctionDefinition()
{
    ASSERT(name != nullptr);
    delete name;

    if (parameter_list)
    {
        delete parameter_list;
    }

    if (return_type)
    {

        delete return_type;
    }

    ASSERT(body != nullptr);
    delete body;
}

AST_VariableDefinition::AST_VariableDefinition(Location const  location,
                                               AST_Identifier *lhs,
                                               AST_Expression *rhs)
    : AST_Node(location), lhs{lhs}, rhs{rhs}
{}

AST_VariableDefinition::~AST_VariableDefinition()
{
    ASSERT(lhs != nullptr);
    ASSERT(rhs != nullptr);

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
    ASSERT(lhs != nullptr);
    ASSERT(rhs != nullptr);

    delete lhs;
    delete rhs;
}

AST_ExpressionList::AST_ExpressionList(Location const      location,
                                       AST_Expression     *expression,
                                       AST_ExpressionList *rest_expressions)
    : AST_Node(location), expression{expression}, rest_expressions{
                                                      rest_expressions}
{}

AST_ExpressionList::~AST_ExpressionList()
{
    ASSERT(expression != nullptr);
    delete expression;

    if (rest_expressions)
    {
        delete rest_expressions;
    }
}

AST_ParameterList::AST_ParameterList(Location const     location,
                                     AST_Identifier    *parameter,
                                     AST_ParameterList *rest_parameters)
    : AST_Node(location), parameter{parameter}, rest_parameters{rest_parameters}
{}

AST_ParameterList::~AST_ParameterList()
{
    delete parameter;
    delete rest_parameters;
}

AST_StatementList::AST_StatementList(Location const     location,
                                     AST_Statement     *statement,
                                     AST_StatementList *rest_statements)
    : AST_Node(location), statement{statement}, rest_statements(rest_statements)
{}

AST_StatementList::~AST_StatementList()
{
    delete statement;
    delete rest_statements;
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

AST_BinaryRelation::AST_BinaryRelation(Location const  location,
                                       AST_Expression *lhs, AST_Expression *rhs,
                                       std::string name, int precedence)
    : AST_Node{location}, lhs{lhs}, rhs{rhs}, name{name}, precedence{precedence}
{}

AST_BinaryRelation::~AST_BinaryRelation()
{
    delete rhs;
    delete lhs;
}

AST_LesserThan::AST_LesserThan(Location const location, AST_Expression *lhs,
                               AST_Expression *rhs)
    : AST_Node{location}, AST_BinaryRelation{location, lhs, rhs, "Lesser than",
                                             4}
{}

AST_LesserThanOrEqual::AST_LesserThanOrEqual(Location const  location,
                                             AST_Expression *lhs,
                                             AST_Expression *rhs)
    : AST_Node{location}, AST_BinaryRelation{location, lhs, rhs,
                                             "Lesser than or equal", 4}
{}

AST_DoubleEquals::AST_DoubleEquals(Location const location, AST_Expression *lhs,
                                   AST_Expression *rhs)
    : AST_Node{location}, AST_BinaryRelation{location, lhs, rhs, "Equality", 4}
{}

AST_GreaterThan::AST_GreaterThan(Location const location, AST_Expression *lhs,
                                 AST_Expression *rhs)
    : AST_Node{location}, AST_BinaryRelation{location, lhs, rhs, "Greater than",
                                             4}
{}

AST_GreaterThanOrEquals::AST_GreaterThanOrEquals(Location const  location,
                                                 AST_Expression *lhs,
                                                 AST_Expression *rhs)
    : AST_Node{location}, AST_BinaryRelation{location, lhs, rhs,
                                             "Greater than or equals", 4}
{}
