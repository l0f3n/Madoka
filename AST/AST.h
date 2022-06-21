#pragma once

#include <SymbolTable/Symbol.h>
#include <SymbolTable/SymbolTable.h>
#include <iostream>
#include <vector>

class Quads;
class AST_Identifier;
class AST_Expression;
class AST_ExpressionList;
class AST_StatementList;

class AST_Node
{
  public:
    virtual ~AST_Node(){};

    std::string indent(std::vector<bool> is_left_history) const;

    void print(std::ostream &os, SymbolTable *symbol_table);

    virtual void print(std::ostream &os, SymbolTable *symbol_table,
                       bool is_left, std::vector<bool> is_left_history) const;

    virtual Symbol *generate_quads(Quads *quads) const;
};

class AST_ParameterList : public AST_Node
{
  public:
    AST_ParameterList(AST_Identifier *last_parameter);
    ~AST_ParameterList();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    void add_parameter(AST_Identifier *parameter);

    Symbol *generate_quads(Quads *quads) const override;

    std::vector<AST_Identifier *> parameters;
};

class AST_Statement : virtual public AST_Node
{};

class AST_If : public AST_Statement
{
  public:
    AST_If(AST_Expression *, AST_StatementList *);
    ~AST_If();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_Expression    *condition;
    AST_StatementList *body;
};

class AST_Return : public AST_Statement
{
  public:
    AST_Return(AST_ExpressionList *);
    ~AST_Return();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_ExpressionList *return_values;
};

class AST_VariableDefinition : public AST_Statement
{
  public:
    AST_VariableDefinition(AST_Identifier *, AST_Expression *);
    ~AST_VariableDefinition();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_Identifier *lhs;
    AST_Expression *rhs;
};

class AST_VariableAssignment : public AST_Statement
{
  public:
    AST_VariableAssignment(AST_Identifier *, AST_Expression *);
    ~AST_VariableAssignment();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_Identifier *lhs;
    AST_Expression *rhs;
};

class AST_ExpressionList : public AST_Node
{
  public:
    AST_ExpressionList(AST_Expression *last_statement);
    ~AST_ExpressionList();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    void add_expression(AST_Expression *expression);

    std::vector<AST_Expression *> expressions;
};

class AST_StatementList : public AST_Statement
{
  public:
    AST_StatementList(AST_Statement *last_statement);
    ~AST_StatementList();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    void add_statement(AST_Statement *statement);

    Symbol *generate_quads(Quads *quads) const override;

    std::vector<AST_Statement *> statements;
};

class AST_FunctionDefinition : public AST_Statement
{
  public:
    AST_FunctionDefinition(AST_Identifier *, AST_ParameterList *,
                           AST_ParameterList *, AST_StatementList *);
    ~AST_FunctionDefinition();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_Identifier    *name;
    AST_ParameterList *parameter_list;
    AST_ParameterList *return_values;
    AST_StatementList *body;
};

class AST_Expression : virtual public AST_Node
{};

class AST_Identifier : public AST_Expression
{
  public:
    AST_Identifier(int symbol_index);

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    int symbol_index;
};

class AST_FunctionCall : public AST_Expression, public AST_Statement
{
  public:
    AST_FunctionCall(AST_Identifier *, AST_ExpressionList *);
    ~AST_FunctionCall();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_Identifier     *ident;
    AST_ExpressionList *arguments;
};

class AST_Integer : public AST_Expression
{
  public:
    AST_Integer(long value);

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    long value;
};

class AST_Real : public AST_Expression
{
  public:
    AST_Real(double value);

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    double value;
};

class AST_UnaryMinus : public AST_Expression
{
  public:
    AST_UnaryMinus(AST_Expression *expr);
    ~AST_UnaryMinus();

    void print(std::ostream &os, SymbolTable *symbol_table, bool is_left,
               std::vector<bool> is_left_history) const override;

    Symbol *generate_quads(Quads *quads) const override;

    AST_Expression *expr;
};

class AST_BinaryOperation : public AST_Expression
{
  public:
    AST_BinaryOperation(AST_Expression *lhs, AST_Expression *rhs,
                        std::string name, int precedence);
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
    AST_Plus(AST_Expression *lhs, AST_Expression *rhs);

    Symbol *generate_quads(Quads *quads) const override;
};

class AST_Minus : public AST_BinaryOperation
{
  public:
    AST_Minus(AST_Expression *lhs, AST_Expression *rhs);

    Symbol *generate_quads(Quads *quads) const override;
};

class AST_Multiplication : public AST_BinaryOperation
{
  public:
    AST_Multiplication(AST_Expression *lhs, AST_Expression *rhs);

    Symbol *generate_quads(Quads *quads) const override;
};

class AST_Division : public AST_BinaryOperation
{
  public:
    AST_Division(AST_Expression *lhs, AST_Expression *rhs);

    Symbol *generate_quads(Quads *quads) const override;
};
