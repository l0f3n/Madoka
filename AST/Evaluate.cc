#include "AST.h"
#include <cmath>

double AST_Node::evaluate(double x) const
{
    std::cout << "Wrong evaluate implementation" << std::endl;
    return 0;
}

double AST_UnaryMinus::evaluate(double x) const { return -expr->evaluate(x); }

double AST_Plus::evaluate(double x) const
{
    return lhs->evaluate(x) + rhs->evaluate(x);
}

double AST_Minus::evaluate(double x) const
{
    return lhs->evaluate(x) - rhs->evaluate(x);
}

double AST_Division::evaluate(double x) const
{
    return lhs->evaluate(x) / rhs->evaluate(x);
}

double AST_Multiplication::evaluate(double x) const
{
    return lhs->evaluate(x) * rhs->evaluate(x);
}

double AST_Exponentiation::evaluate(double x) const
{
    return pow(lhs->evaluate(x), rhs->evaluate(x));
}

double AST_FunctionCall::evaluate(double x) const
{
    double      value = expr->evaluate(x);
    std::string name  = ident->name;

    if (name == "sin")
    {
        return sin(value);
    }
    else if (name == "cos")
    {
        return cos(value);
    }
    else if (name == "tan")
    {
        return tan(value);
    }
    else if (name == "log")
    {
        return log(value);
    }
    else if (name == "abs")
    {
        return fabs(value);
    }
    else if (name == "sqrt")
    {
        return sqrt(value);
    }
    else if (name == "ceil")
    {
        return ceilf(value);
    }
    else if (name == "floor")
    {
        return floorf(value);
    }
    else if (name == "round")
    {
        return roundf(value);
    }
    else
    {
        std::cout << "Unknown function: " << name << std::endl;
        return value;
    }
}

double AST_Number::evaluate(double x) const { return value; }

double AST_Identifier::evaluate(double x) const
{
    if (name == "x")
    {
        return x;
    }
    else
    {
        std::cout << "Unknown variable: " << name << std::endl;
        return 0;
    }
}
