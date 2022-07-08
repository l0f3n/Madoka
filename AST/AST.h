#pragma once

#include <SymbolTable/Symbol.h>
#include <SymbolTable/SymbolTable.h>
#include <iostream>
#include <vector>

class Quads;
class TypeChecker;

class AST_Identifier;
class AST_Expression;
class AST_ExpressionList;
class AST_StatementList;

class AST_Node
{
  public:
    AST_Node(Location const location);
    virtual ~AST_Node(){};

    std::string indent(std::vector<bool> is_left_history) const;

    void print(std::ostream &os, SymbolTable *symbol_table);

    virtual void print(std::ostream &os, SymbolTable *symbol_table,
                       bool              is_left,
                       std::vector<bool> is_left_history) const = 0;

    virtual int generate_quads(Quads *quads) const          = 0;
    virtual int type_check(TypeChecker *type_checker) const = 0;

    Location const location;
};

class AST_ParameterList : public AST_Node
{
  public:
    AST_ParameterList(Location const, AST_Identifier *, AST_ParameterList *);
    ~AST_ParameterList();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Identifier    *parameter;
    AST_ParameterList *rest_parameters;
};

class AST_Statement : virtual public AST_Node
{};

class AST_If : public AST_Statement
{
  public:
    AST_If(Location const, AST_Expression *, AST_StatementList *);
    ~AST_If();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Expression    *condition;
    AST_StatementList *body;
};

class AST_Return : public AST_Statement
{
  public:
    AST_Return(Location const, AST_Expression *);
    ~AST_Return();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Expression *expression;
};

class AST_VariableDefinition : public AST_Statement
{
  public:
    AST_VariableDefinition(Location const, AST_Identifier *, AST_Expression *);
    ~AST_VariableDefinition();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Identifier *lhs;
    AST_Expression *rhs;
};

class AST_VariableAssignment : public AST_Statement
{
  public:
    AST_VariableAssignment(Location const, AST_Identifier *, AST_Expression *);
    ~AST_VariableAssignment();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int type_check(TypeChecker *type_checker) const override;
    int generate_quads(Quads *quads) const override;

    AST_Identifier *lhs;
    AST_Expression *rhs;
};

class AST_ExpressionList : public AST_Node
{
  public:
    AST_ExpressionList(Location const, AST_Expression *, AST_ExpressionList *);
    ~AST_ExpressionList();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int type_check(TypeChecker *type_checker) const override;
    int generate_quads(Quads *quads) const override;
    int generate_quads_parameter_list(Quads *quads) const;

    AST_Expression     *expression;
    AST_ExpressionList *rest_expressions;
};

class AST_StatementList : public AST_Statement
{
  public:
    AST_StatementList(Location const, AST_Statement *, AST_StatementList *);
    ~AST_StatementList();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Statement     *statement;
    AST_StatementList *rest_statements;
};

class AST_FunctionDefinition : public AST_Statement
{
  public:
    AST_FunctionDefinition(Location const, AST_Identifier *,
                           AST_ParameterList *, AST_Identifier *,
                           AST_StatementList *);
    ~AST_FunctionDefinition();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Identifier    *name;
    AST_ParameterList *parameter_list;
    AST_Identifier    *return_type;
    AST_StatementList *body;
};

class AST_Expression : virtual public AST_Node
{};

class AST_Identifier : public AST_Expression
{
  public:
    AST_Identifier(Location const, int symbol_index);

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    int symbol_index;
};

class AST_FunctionCall : public AST_Expression, public AST_Statement
{
  public:
    AST_FunctionCall(Location const, AST_Identifier *, AST_ExpressionList *);
    ~AST_FunctionCall();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Identifier     *ident;
    AST_ExpressionList *arguments;
};

class AST_Integer : public AST_Expression
{
  public:
    AST_Integer(Location const, long value);

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    long value;
};

class AST_Real : public AST_Expression
{
  public:
    AST_Real(Location const, double value);

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    double value;
};

class AST_UnaryMinus : public AST_Expression
{
  public:
    AST_UnaryMinus(Location const, AST_Expression *expr);
    ~AST_UnaryMinus();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;

    AST_Expression *expr;
};

class AST_BinaryOperation : public AST_Expression
{
  public:
    AST_BinaryOperation(Location const, AST_Expression *lhs,
                        AST_Expression *rhs, std::string name, int precedence);
    virtual ~AST_BinaryOperation();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    AST_Expression *lhs;
    AST_Expression *rhs;

    std::string name{""};
    int         precedence;
};

class AST_Plus : public AST_BinaryOperation
{
  public:
    AST_Plus(Location const, AST_Expression *lhs, AST_Expression *rhs);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_Minus : public AST_BinaryOperation
{
  public:
    AST_Minus(Location const, AST_Expression *lhs, AST_Expression *rhs);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_Multiplication : public AST_BinaryOperation
{
  public:
    AST_Multiplication(Location const, AST_Expression *lhs,
                       AST_Expression *rhs);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_Division : public AST_BinaryOperation
{
  public:
    AST_Division(Location const, AST_Expression *lhs, AST_Expression *rhs);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_BinaryRelation : public AST_Expression
{
  public:
    AST_BinaryRelation(Location const, AST_Expression *lhs, AST_Expression *rhs,
                       std::string name, int precedence);
    ~AST_BinaryRelation();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    AST_Expression *lhs;
    AST_Expression *rhs;

    std::string name{""};
    int         precedence;
};

class AST_LesserThan : public AST_BinaryRelation
{
  public:
    AST_LesserThan(Location const, AST_Expression *, AST_Expression *);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_LesserThanOrEqual : public AST_BinaryRelation
{
  public:
    AST_LesserThanOrEqual(Location const, AST_Expression *, AST_Expression *);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_DoubleEquals : public AST_BinaryRelation
{
  public:
    AST_DoubleEquals(Location const, AST_Expression *, AST_Expression *);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_GreaterThan : public AST_BinaryRelation
{
  public:
    AST_GreaterThan(Location const, AST_Expression *, AST_Expression *);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};

class AST_GreaterThanOrEquals : public AST_BinaryRelation
{
  public:
    AST_GreaterThanOrEquals(Location const, AST_Expression *, AST_Expression *);

    int generate_quads(Quads *quads) const override;
    int type_check(TypeChecker *type_checker) const override;
};
