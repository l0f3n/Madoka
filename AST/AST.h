#pragma once

#include <SymbolTable/Symbol.h>
#include <iostream>
#include <vector>

class Quads;

class AST_Node
{
  public:
    virtual ~AST_Node(){};

    std::string indent(std::vector<bool> is_left_history) const;

    void            print(std::ostream &os);
    virtual void    print(std::ostream &os, bool is_left,
                          std::vector<bool> is_left_history) const;
    virtual Symbol *generate_quads(Quads *quads) const;
};

class AST_Expression : public AST_Node
{};

class AST_Identifier : public AST_Expression
{
  public:
    AST_Identifier(Symbol *symbol, std::string name);

    void    print(std::ostream &os, bool is_left,
                  std::vector<bool> is_left_history) const override;
    Symbol *generate_quads(Quads *quads) const override;

    Symbol     *symbol;
    std::string name;
};

class AST_FunctionCall : public AST_Expression
{
  public:
    AST_FunctionCall(Symbol *symbol, AST_Identifier *ident,
                     AST_Expression *expr);
    ~AST_FunctionCall();

    void    print(std::ostream &os, bool is_left,
                  std::vector<bool> is_left_history) const override;
    Symbol *generate_quads(Quads *quads) const override;

    Symbol         *symbol;
    AST_Identifier *ident;
    AST_Expression *expr;
};

class AST_Integer : public AST_Expression
{
  public:
    AST_Integer(long value);

    void    print(std::ostream &os, bool is_left,
                  std::vector<bool> is_left_history) const override;
    Symbol *generate_quads(Quads *quads) const override;

    long value;
};

class AST_Real : public AST_Expression
{
  public:
    AST_Real(double value);

    void    print(std::ostream &os, bool is_left,
                  std::vector<bool> is_left_history) const override;
    Symbol *generate_quads(Quads *quads) const override;

    double value;
};

class AST_UnaryMinus : public AST_Expression
{
  public:
    AST_UnaryMinus(AST_Expression *expr);
    ~AST_UnaryMinus();

    void    print(std::ostream &os, bool is_left,
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

    void print(std::ostream &os, bool is_left,
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
