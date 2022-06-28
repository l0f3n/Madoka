#include "TypeChecker.h"
#include "AST/AST.h"
#include "Error/Error.h"
#include "SymbolTable/Symbol.h"

TypeChecker::TypeChecker(SymbolTable *const symbol_table)
    : symbol_table{symbol_table}
{}

void TypeChecker::type_check(AST_Node const *const root)
{
    root->type_check(this);
}

void TypeChecker::type_check_arguments(AST_FunctionCall const *function_call,
                                       int                     parameter_index,
                                       AST_ExpressionList     *arguments)
{
    if (parameter_index == -1 && arguments == nullptr)
    {
        // NOTE: If both arguments and parameters stop at the same time its fine
        return;
    }

    else if (parameter_index != -1 && arguments == nullptr)
    {
        std::string function_name =
            symbol_table->get_symbol(function_call->ident->symbol_index)->name;

        report_type_error(function_call->location,
                          "Too few arguments in call to function '" +
                              function_name + "'");
    }

    else if (parameter_index == -1 && arguments != nullptr)
    {
        std::string function_name =
            symbol_table->get_symbol(function_call->ident->symbol_index)->name;

        report_type_error(function_call->location,
                          "Too many arguments in call to function '" +
                              function_name + "'");
    }

    ParameterSymbol *parameter =
        symbol_table->get_parameter_symbol(parameter_index);

    AST_Expression *argument      = arguments->expression;
    int             argument_type = argument->type_check(this);

    if (parameter->type != argument_type)
    {
        std::string parameter_name = parameter->name;

        std::string function_name =
            symbol_table->get_symbol(function_call->ident->symbol_index)->name;

        std::string parameter_type_name =
            symbol_table->get_type_name(parameter->type);

        std::string argument_type_name =
            symbol_table->get_type_name(argument_type);

        report_type_error(argument->location,
                          "Parameter '" + parameter_name + "' to function '" +
                              function_name + "' expects type '" +
                              parameter_type_name + "', but got type '" +
                              argument_type_name + "'");
    }

    type_check_arguments(function_call, parameter->next_parameter,
                         arguments->rest_expressions);
}

// =====================================
// ===== Definition of AST methods =====
// =====================================

int AST_ParameterList::type_check(TypeChecker *type_checker) const
{
    report_internal_compiler_error(
        "AST_ParameterList::type_check() should not be called");

    return -1;
}

int AST_If::type_check(TypeChecker *type_checker) const { return -1; }

int AST_Return::type_check(TypeChecker *type_checker) const
{
    // TODO: Check that what we return matches what the function is suppose to
    // return, both the NUMBER and TYPES of return values
    return -1;
}

int AST_VariableDefinition::type_check(TypeChecker *type_checker) const
{
    int lhs_type = lhs->type_check(type_checker);
    int rhs_type = rhs->type_check(type_checker);

    if (lhs_type != rhs_type)
    {
        Symbol *symbol =
            type_checker->symbol_table->get_symbol(lhs->symbol_index);

        std::string m = "Variable '" + symbol->name + "' with type '" +
                        type_checker->symbol_table->get_type_name(lhs_type) +
                        "' cannot be assigned value of type '" +
                        type_checker->symbol_table->get_type_name(rhs_type) +
                        "'";

        report_type_error(location, m);
    }

    return -1;
}

int AST_VariableAssignment::type_check(TypeChecker *type_checker) const
{
    int lhs_type = lhs->type_check(type_checker);
    int rhs_type = rhs->type_check(type_checker);

    if (lhs_type != rhs_type)
    {
        Symbol *symbol =
            type_checker->symbol_table->get_symbol(lhs->symbol_index);

        std::string m = "Variable '" + symbol->name + "' with type '" +
                        type_checker->symbol_table->get_type_name(lhs_type) +
                        "' cannot be assigned value of type '" +
                        type_checker->symbol_table->get_type_name(rhs_type) +
                        "'";

        report_type_error(location, m);
    }

    return -1;
}

int AST_ExpressionList::type_check(TypeChecker *type_checker) const
{
    report_internal_compiler_error(
        "AST_ExpressionList::type_check() should not be called");
    return -1;
}

int AST_StatementList::type_check(TypeChecker *type_checker) const
{
    ASSERT(statement != nullptr);
    statement->type_check(type_checker);

    if (rest_statements)
    {
        rest_statements->type_check(type_checker);
    }

    return -1;
}

int AST_FunctionDefinition::type_check(TypeChecker *type_checker) const
{
    // NOTE: We do not need to type check parameters

    /*
    if (return_values)
    {
        return_values->type_check(type_checker);
    }
    */

    ASSERT(body != nullptr);
    body->type_check(type_checker);

    return -1;
}

int AST_Identifier::type_check(TypeChecker *type_checker) const
{
    Symbol *symbol = type_checker->symbol_table->get_symbol(symbol_index);

    // TODO: If we would typecheck a type, it would return type void, this could
    // potentially lead to some problems
    ASSERT(symbol->type != type_checker->symbol_table->type_void);

    return symbol->type;
}

int AST_FunctionCall::type_check(TypeChecker *type_checker) const
{

    FunctionSymbol *function =
        type_checker->symbol_table->get_function_symbol(ident->symbol_index);

    type_checker->type_check_arguments(this, function->first_parameter,
                                       arguments);

    // TODO: Since we want functions to be able to return multiple things,
    // we need to handle that in some way. We simply cant say that the
    // function has type 'real' for that reason, its needs something like
    // 'real, real', but how do we implement that, but how do we implement
    // that?

    // TODO: For now, lets just return integer for simplicity
    return type_checker->symbol_table->type_integer;
}

int AST_Integer::type_check(TypeChecker *type_checker) const
{
    return type_checker->symbol_table->type_integer;
}

int AST_Real::type_check(TypeChecker *type_checker) const
{
    return type_checker->symbol_table->type_real;
}

int AST_UnaryMinus::type_check(TypeChecker *type_checker) const
{
    return expr->type_check(type_checker);
}

int AST_Plus::type_check(TypeChecker *type_checker) const
{

    int lhs_type = lhs->type_check(type_checker);
    int rhs_type = rhs->type_check(type_checker);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name =
            type_checker->symbol_table->get_type_name(lhs_type);

        std::string rhs_type_name =
            type_checker->symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot add values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}

int AST_Minus::type_check(TypeChecker *type_checker) const
{
    int lhs_type = lhs->type_check(type_checker);
    int rhs_type = rhs->type_check(type_checker);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name =
            type_checker->symbol_table->get_type_name(lhs_type);

        std::string rhs_type_name =
            type_checker->symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot subtract values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}

int AST_Multiplication::type_check(TypeChecker *type_checker) const
{
    int lhs_type = lhs->type_check(type_checker);
    int rhs_type = rhs->type_check(type_checker);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name =
            type_checker->symbol_table->get_type_name(lhs_type);

        std::string rhs_type_name =
            type_checker->symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot multiply values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}

int AST_Division::type_check(TypeChecker *type_checker) const
{
    int lhs_type = lhs->type_check(type_checker);
    int rhs_type = rhs->type_check(type_checker);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name =
            type_checker->symbol_table->get_type_name(lhs_type);

        std::string rhs_type_name =
            type_checker->symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot divide values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}
