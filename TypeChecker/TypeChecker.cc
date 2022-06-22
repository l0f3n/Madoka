#include "TypeChecker.h"
#include "AST/AST.h"
#include "Error/Error.h"

TypeChecker::TypeChecker(SymbolTable *const symbol_table)
    : symbol_table(symbol_table)
{}

void TypeChecker::type_check(AST_Node const *const root)
{
    root->type_check(symbol_table);
}

// =====================================
// ===== Definition of AST methods =====
// =====================================

int AST_ParameterList::type_check(SymbolTable *symbol_table) const
{
    // TODO: Check that these matches the NUMBER and TYPES of arguments to the
    // function we want to call

    // TODO: Make it possible to iterate over statements using for each loop
    for (auto it{parameters.rbegin()}; it != parameters.rend();)
    {
        (*it++)->type_check(symbol_table);
    }

    return -1;
}

int AST_If::type_check(SymbolTable *symbol_table) const { return -1; }

int AST_Return::type_check(SymbolTable *symbol_table) const
{
    // TODO: Check that what we return matches what the function is suppose to
    // return, both the NUMBER and TYPES of return values
    return -1;
}

int AST_VariableDefinition::type_check(SymbolTable *symbol_table) const
{
    int lhs_type = lhs->type_check(symbol_table);
    int rhs_type = rhs->type_check(symbol_table);

    if (lhs_type != rhs_type)
    {
        Symbol *symbol = symbol_table->get_symbol(lhs->symbol_index);

        std::string m = "Variable '" + symbol->name + "' with type '" +
                        symbol_table->get_type_name(lhs_type) +
                        "' cannot be assigned value of type '" +
                        symbol_table->get_type_name(rhs_type) + "'";

        report_type_error(location, m);
    }

    return -1;
}

int AST_VariableAssignment::type_check(SymbolTable *symbol_table) const
{
    int lhs_type = lhs->type_check(symbol_table);
    int rhs_type = rhs->type_check(symbol_table);

    if (lhs_type != rhs_type)
    {
        Symbol *symbol = symbol_table->get_symbol(lhs->symbol_index);

        std::string m = "Variable '" + symbol->name + "' with type '" +
                        symbol_table->get_type_name(lhs_type) +
                        "' cannot be assigned value of type '" +
                        symbol_table->get_type_name(rhs_type) + "'";

        report_type_error(location, m);
    }

    return -1;
}

int AST_ExpressionList::type_check(SymbolTable *symbol_table) const
{
    for (auto it{expressions.rbegin()}; it != expressions.rend();)
    {
        (*it++)->type_check(symbol_table);
    }

    return -1;
}

int AST_StatementList::type_check(SymbolTable *symbol_table) const
{
    // TODO: Make it possible to iterate over statements using for each loop
    for (auto it{statements.rbegin()}; it != statements.rend();)
    {
        (*it++)->type_check(symbol_table);
    }

    return -1;
}

int AST_FunctionDefinition::type_check(SymbolTable *symbol_table) const
{
    if (parameter_list)
    {
        parameter_list->type_check(symbol_table);
    }

    if (return_values)
    {
        return_values->type_check(symbol_table);
    }

    ASSERT(body != nullptr);
    body->type_check(symbol_table);

    return -1;
}

int AST_Identifier::type_check(SymbolTable *symbol_table) const
{
    Symbol *symbol = symbol_table->get_symbol(symbol_index);

    // TODO: If we would typecheck a type, it would return type void, this could
    // potentially lead to some problems
    ASSERT(symbol->type != symbol_table->type_void);

    return symbol->type;
}

int AST_FunctionCall::type_check(SymbolTable *symbol_table) const
{

    // TODO: Check that the function is called with the correct amount and types
    // of parameters

    // TODO: Since we want functions to be able to return multiple things, we
    // need to handle that in some way. We simply cant say that the function has
    // type 'real' for that reason, its needs something like 'real, real', but
    // how do we implement that, but how do we implement that?

    report_type_error(location,
                      "Type check for FUNCTION_CALL not implemented yet");

    return -1;
}

int AST_Integer::type_check(SymbolTable *symbol_table) const
{
    return symbol_table->type_integer;
}

int AST_Real::type_check(SymbolTable *symbol_table) const
{
    return symbol_table->type_real;
}

int AST_UnaryMinus::type_check(SymbolTable *symbol_table) const
{
    return expr->type_check(symbol_table);
}

int AST_Plus::type_check(SymbolTable *symbol_table) const
{

    int lhs_type = lhs->type_check(symbol_table);
    int rhs_type = rhs->type_check(symbol_table);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name = symbol_table->get_type_name(lhs_type);
        std::string rhs_type_name = symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot add values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}

int AST_Minus::type_check(SymbolTable *symbol_table) const
{
    int lhs_type = lhs->type_check(symbol_table);
    int rhs_type = rhs->type_check(symbol_table);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name = symbol_table->get_type_name(lhs_type);
        std::string rhs_type_name = symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot subtract values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}

int AST_Multiplication::type_check(SymbolTable *symbol_table) const
{
    int lhs_type = lhs->type_check(symbol_table);
    int rhs_type = rhs->type_check(symbol_table);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name = symbol_table->get_type_name(lhs_type);
        std::string rhs_type_name = symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot multiply values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}

int AST_Division::type_check(SymbolTable *symbol_table) const
{
    int lhs_type = lhs->type_check(symbol_table);
    int rhs_type = rhs->type_check(symbol_table);

    if (lhs_type != rhs_type)
    {
        std::string lhs_type_name = symbol_table->get_type_name(lhs_type);
        std::string rhs_type_name = symbol_table->get_type_name(rhs_type);

        report_type_error(location, "Cannot divide values of type '" +
                                        lhs_type_name + "' and '" +
                                        rhs_type_name + "'");
    }

    return lhs_type;
}
