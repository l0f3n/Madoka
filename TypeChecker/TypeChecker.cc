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
        // NOTE: If both arguments and parameters stop at the same time there is
        // equally many of them, so its all good
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
            symbol_table->get_name(parameter->type);

        std::string argument_type_name = symbol_table->get_name(argument_type);

        report_type_error(argument->location,
                          "Parameter '" + parameter_name + "' to function '" +
                              function_name + "' expects type '" +
                              parameter_type_name + "', but got type '" +
                              argument_type_name + "'");
    }

    type_check_arguments(function_call, parameter->next_parameter,
                         arguments->rest_expressions);
}

int TypeChecker::type_check_binary_operation(
    AST_BinaryOperation const *binary_operation)
{
    AST_Expression *lhs = binary_operation->lhs;
    AST_Expression *rhs = binary_operation->rhs;

    int lhs_type = lhs->type_check(this);
    int rhs_type = rhs->type_check(this);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name = symbol_table->get_name(lhs_type);
        std::string rhs_type_name = symbol_table->get_name(rhs_type);

        report_type_error(binary_operation->location,
                          "Cannot use operation '" + binary_operation->name +
                              "' on values of different types '" +
                              lhs_type_name + "' and '" + rhs_type_name + "'");
    }

    // NOTE: lhs and rhs are the same type, so it doesn't matter which one we
    // return
    return lhs_type;
}

int TypeChecker::type_check_binary_relation(
    AST_BinaryRelation const *binary_relation)
{
    AST_Expression *lhs = binary_relation->lhs;
    AST_Expression *rhs = binary_relation->rhs;

    int lhs_type = lhs->type_check(this);
    int rhs_type = rhs->type_check(this);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name = symbol_table->get_name(lhs_type);
        std::string rhs_type_name = symbol_table->get_name(rhs_type);

        report_type_error(binary_relation->location,
                          "Cannot use comparison '" + binary_relation->name +
                              "' on values of different types '" +
                              lhs_type_name + "' and '" + rhs_type_name + "'");
    }

    // NOTE: Comparisons always return bool
    return symbol_table->type_bool;
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

int AST_If::type_check(TypeChecker *type_checker) const
{
    ASSERT(condition != nullptr);
    int condition_type = condition->type_check(type_checker);

    if (condition_type != type_checker->symbol_table->type_bool)
    {
        std::string type_name =
            type_checker->symbol_table->get_name(condition_type);

        report_type_error(
            location,
            "If statement condition expected type 'bool', got type '" +
                type_name + "'");
    }

    ASSERT(body != nullptr);

    body->type_check(type_checker);

    return -1;
}

int AST_Return::type_check(TypeChecker *type_checker) const
{
    FunctionSymbol *function = type_checker->symbol_table->get_function_symbol(
        type_checker->symbol_table->enclosing_scope());

    int actual_type;
    if (expression != nullptr)
    {
        actual_type = expression->type_check(type_checker);
    }
    else
    {
        actual_type = type_checker->symbol_table->type_void;
    }

    if (actual_type != function->type)
    {
        std::string actual_type_name =
            type_checker->symbol_table->get_name(actual_type);

        std::string function_name = function->name;

        std::string formal_type_name =
            type_checker->symbol_table->get_name(function->type);

        report_type_error(location, "Cannot return type '" + actual_type_name +
                                        "' from function '" + function_name +
                                        "' that is suppose to return type '" +
                                        formal_type_name + "'");
    }

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
                        type_checker->symbol_table->get_name(lhs_type) +
                        "' cannot be assigned value of type '" +
                        type_checker->symbol_table->get_name(rhs_type) + "'";

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
                        type_checker->symbol_table->get_name(lhs_type) +
                        "' cannot be assigned value of type '" +
                        type_checker->symbol_table->get_name(rhs_type) + "'";

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

    FunctionSymbol *function =
        type_checker->symbol_table->get_function_symbol(name->symbol_index);

    // NOTE: We can only implicitly return from a function if it returns nothing
    if (!function->has_return &&
        function->type != type_checker->symbol_table->type_void)
    {
        std::string formal_type_name =
            type_checker->symbol_table->get_name(function->type);

        report_type_error(location, "Function '" + function->name +
                                        "' is suppose to return type '" +
                                        formal_type_name +
                                        "', but doesn't return anything");
    }

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

    return function->type;
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
    int type = expr->type_check(type_checker);

    if (type != type_checker->symbol_table->type_integer &&
        type != type_checker->symbol_table->type_real)
    {
        report_type_error(
            location,
            "Unary minus expected type 'int' or type 'real', not type '" +
                type_checker->symbol_table->get_name(type) + "'");
    }

    return type;
}

int AST_Plus::type_check(TypeChecker *type_checker) const
{
    return type_checker->type_check_binary_operation(this);
}

int AST_Minus::type_check(TypeChecker *type_checker) const
{
    return type_checker->type_check_binary_operation(this);
}

int AST_Multiplication::type_check(TypeChecker *type_checker) const
{
    return type_checker->type_check_binary_operation(this);
}

int AST_Division::type_check(TypeChecker *type_checker) const
{
    return type_checker->type_check_binary_operation(this);
}

int AST_GreaterThan::type_check(TypeChecker *type_checker) const
{

    return type_checker->type_check_binary_relation(this);
}
