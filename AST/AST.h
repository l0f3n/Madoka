#pragma once

#include <iostream>
#include <vector>

class AST_Node
{
  public:
    void print(std::ostream &os);
    virtual ~AST_Node(){};

    virtual double evaluate(double x) const;

    virtual void print(std::ostream &os, bool is_left,
                       std::vector<bool> is_left_history) const;
    std::string  indent(std::vector<bool> is_left_history) const;
};

class AST_Expression : public AST_Node
{};

class AST_Identifier : public AST_Expression
{
  public:
    AST_Identifier(std::string name);

    double evaluate(double x) const override;

    void print(std::ostream &os, bool is_left,
               std::vector<bool> is_left_history) const override;

    std::string name;
};

class AST_FunctionCall : public AST_Expression
{
  public:
    AST_FunctionCall(AST_Identifier *ident, AST_Expression *expr);
    ~AST_FunctionCall();

    double evaluate(double x) const override;

    void print(std::ostream &os, bool is_left,
               std::vector<bool> is_left_history) const override;

    AST_Identifier *ident;
    AST_Expression *expr;
};

class AST_Number : public AST_Expression
{
  public:
    AST_Number(double value);

    double evaluate(double x) const override;

    void print(std::ostream &os, bool is_left,
               std::vector<bool> is_left_history) const override;

    double value;
};

class AST_UnaryMinus : public AST_Expression
{
  public:
    AST_UnaryMinus(AST_Expression *expr);
    ~AST_UnaryMinus();

    double evaluate(double x) const override;

    void print(std::ostream &os, bool is_left,
               std::vector<bool> is_left_history) const override;

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

    double evaluate(double x) const override;
};

class AST_Minus : public AST_BinaryOperation
{
  public:
    AST_Minus(AST_Expression *lhs, AST_Expression *rhs);

    double evaluate(double x) const override;
};

class AST_Multiplication : public AST_BinaryOperation
{
  public:
    AST_Multiplication(AST_Expression *lhs, AST_Expression *rhs);

    double evaluate(double x) const override;
};

class AST_Division : public AST_BinaryOperation
{
  public:
    AST_Division(AST_Expression *lhs, AST_Expression *rhs);

    double evaluate(double x) const override;
};

class AST_Exponentiation : public AST_BinaryOperation
{
  public:
    AST_Exponentiation(AST_Expression *lhs, AST_Expression *rhs);

    double evaluate(double x) const override;
};
