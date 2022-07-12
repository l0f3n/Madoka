#include "Parser.h"
#include "AST/AST.h"
#include "CodeGenerator/CodeGenerator.h"
#include "Error/Error.h"
#include "Quads/Quads.h"
#include "SymbolTable/Symbol.h"
#include "Tokenizer/Tokenizer.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

Parser::Parser(Tokenizer &tokenizer, SymbolTable *symbol_table,
               TypeChecker &type_checker, Quads &quads,
               CodeGenerator &code_generator)
    : tokenizer{tokenizer}, symbol_table{symbol_table},
      type_checker{type_checker}, quads{quads}, code_generator{code_generator}
{
    ASSERT(symbol_table != nullptr);
}

Token Parser::expect(Token::Kind kind)
{
    Token next{tokenizer.peek(1)};

    if (next.kind == kind)
    {
        return tokenizer.eat();
    }
    else
    {
        report_parse_error_unexpected_token(next);
        return next;
    }
}

AST_BinaryOperation *Parser::respect_precedence(AST_BinaryOperation *binop)
{
    // Reorganize tree so that in respects operator precedence. We may need to
    // reorder it as follows:
    //
    //      *              +
    //     / \            / \
    //    1   +    ->    *   3
    //       / \        / \
    //      2   3      1   2
    //
    // Of course, 1, 2 and 3 in this example may contain arbitrarily complex
    // subtrees. But we do this reorganization from the bottom up so every
    // sub-tree is also guaranteed to be correct.

    ASSERT(binop != nullptr);

    AST_BinaryOperation *binop_below =
        dynamic_cast<AST_BinaryOperation *>(binop->rhs);

    if (binop_below != nullptr && binop->precedence > binop_below->precedence)
    {
        binop->rhs       = binop_below->lhs;
        binop_below->lhs = binop;
        return binop_below;
    }
    else
    {
        return binop;
    }
}

AST_Node *Parser::parse()
{
    // NOTE: Before we begin parsing the user program, we add all predefined
    // functions to the symbol table to make sure the user code can reference
    // them. We also generate code for these functions of course. This solution
    // seems really weird to me since the parser and the code generator will be
    // coupled in a strange, and we don't utilise any typechecking or anything.

    Location no_location{-1, -1, -1, -1};

    // NOTE: Here we create all the callable print functions for the various
    // types

    // NOTE: Print integer
    symbol_table->insert_function(
        no_location, "print#" + std::to_string(symbol_table->type_integer));
    symbol_table->open_scope();
    symbol_table->insert_parameter(no_location, "message",
                                   symbol_table->type_integer);
    symbol_table->close_scope();

    // NOTE: Print bool
    symbol_table->insert_function(
        no_location, "print#" + std::to_string(symbol_table->type_bool));
    symbol_table->open_scope();
    symbol_table->insert_parameter(no_location, "message",
                                   symbol_table->type_bool);
    symbol_table->close_scope();

    code_generator.generate_predefined_functions();

    AST_Node *expr = parse_statement_list();

    expect(Token::Kind::End);

    // NOTE: We have a function called '#global' that runs automatically
    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    // NOTE: This feels really weird. Manually generating an AST for a function
    // called '#global' and making it call 'main'. Think about doing it some
    // other way

    // TODO: Probably create constant to refer to #global
    AST_Identifier *name =
        new AST_Identifier(no_location, symbol_table->lookup_symbol("#global"));

    int symbol_index = symbol_table->lookup_symbol("main");
    if (symbol_index == -1)
    {
        report_parse_error(no_location, "Function 'main' needs to be defined");
    }

    Symbol *symbol = symbol_table->get_symbol(symbol_index);
    if (symbol->tag != Symbol::Tag::Function)
    {
        report_parse_error(symbol->location,
                           "Cannot define variable with name 'main'");
    }

    AST_Identifier *main = new AST_Identifier(no_location, symbol_index);

    AST_FunctionCall *call = new AST_FunctionCall(no_location, main, nullptr);

    AST_StatementList *body = new AST_StatementList(no_location, call, nullptr);

    AST_Node *default_function =
        new AST_FunctionDefinition(no_location, name, nullptr, nullptr, body);

    quads.generate_quads(default_function);
    code_generator.generate_code(quads);

    // NOTE: We are done, so this is not necessary. Just do it for closure.
    symbol_table->close_scope();

    return expr;
}

AST_StatementList *Parser::parse_statement_list()
{
    AST_Statement *statement = parse_statement();
    ASSERT(statement != nullptr);

    AST_StatementList *statement_list;
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Identifier:
    case Token::Kind::Return:
    case Token::Kind::Function:
    case Token::Kind::LeftCurlyBrace:
    case Token::Kind::If:
    case Token::Kind::For:
    {
        statement_list = parse_statement_list();
        break;
    }
    default:
    {
        statement_list = nullptr;
        break;
    }
    }

    return new AST_StatementList(statement->location, statement,
                                 statement_list);
}

AST_Statement *Parser::parse_statement()
{
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Identifier:
    {
        switch (tokenizer.peek(2).kind)
        {
        // Definition
        case Token::Kind::Colon:
        {
            Token name_token = tokenizer.eat();

            tokenizer.eat();

            Token type_token = expect(Token::Kind::Identifier);

            expect(Token::Kind::Equals);

            AST_Expression *expression = parse_expression();

            // NOTE: We are done parsing the definition, now we can do
            // all checks for correctness

            int type_index = symbol_table->lookup_symbol(type_token.text);

            if (type_index == -1)
            {
                report_parse_error(type_token.location,
                                   "Unknown type '" + type_token.text + "'");
            }
            else if (symbol_table->get_symbol(type_index)->tag !=
                     Symbol::Tag::Type)
            {
                report_parse_error(type_token.location, "Symbol '" +
                                                            type_token.text +
                                                            "' is not a type");
            }

            int symbol_index = symbol_table->insert_variable(
                name_token.location, name_token.text, type_index);

            AST_Identifier *identifier =
                new AST_Identifier(name_token.location, symbol_index);

            return new AST_VariableDefinition(name_token.location, identifier,
                                              expression);
        }
        // Assignment
        case Token::Kind::Equals:
        {
            Token name_token = tokenizer.eat();

            tokenizer.eat();

            int symbol_index = symbol_table->lookup_symbol(name_token.text);

            if (symbol_index == -1)
            {
                report_parse_error_undefined_reference(name_token);
            }

            AST_Identifier *ident =
                new AST_Identifier(name_token.location, symbol_index);

            AST_Expression *expression = parse_expression();

            return new AST_VariableAssignment(name_token.location, ident,
                                              expression);
        }
        // Function call
        case Token::Kind::LeftParentheses:
        {
            return parse_function_call();
        }
        default:
        {
            report_parse_error_unexpected_token(tokenizer.peek(1));
        }
        }
    }
    case Token::Kind::Return:
    {
        Token token_return = tokenizer.eat();

        AST_Expression *expression = parse_optional_expression();

        // TODO: We need to handle the case where there are multiple branching
        // paths in the body, and check if every path has a return. But we
        // haven't implemented If statements yet.

        FunctionSymbol *function =
            symbol_table->get_function_symbol(symbol_table->enclosing_scope());

        function->has_return = true;

        return new AST_Return(token_return.location, expression);
    }
    // Function definition
    case Token::Kind::Function:
    {
        Token token_function = tokenizer.eat();

        Token name_token = expect(Token::Kind::Identifier);

        int symbol_index =
            symbol_table->insert_function(name_token.location, name_token.text);

        symbol_table->open_scope();

        AST_Identifier *name =
            new AST_Identifier(name_token.location, symbol_index);

        expect(Token::Kind::LeftParentheses);

        AST_ParameterList *parameter_list = parse_optional_parameter_list();

        expect(Token::Kind::RightParentheses);

        // NOTE: We have finished parsing the parameters and are ready to update
        // the name of the function to include those parameters

        FunctionSymbol *function =
            symbol_table->get_function_symbol(symbol_index);

        std::ostringstream oss{};
        oss << function->name;

        if (function->first_parameter != -1)
        {

            ParameterSymbol *parameter =
                symbol_table->get_parameter_symbol(function->first_parameter);

            oss << "#" << parameter->type;

            while (parameter->next_parameter != -1)
            {
                parameter = symbol_table->get_parameter_symbol(
                    parameter->next_parameter);
                oss << "#" << parameter->type;
            }
        }

        symbol_table->update_name(symbol_index, oss.str());

        AST_Identifier *return_type = parse_optional_return();

        // NOTE: The default return type is void, but if we find an explicit
        // return here we need to set it to that instead
        if (return_type)
        {
            FunctionSymbol *function =
                symbol_table->get_function_symbol(symbol_index);

            function->type = return_type->symbol_index;
        }

        expect(Token::Kind::LeftCurlyBrace);

        // TODO: Should empty function bodies be allowed?
        AST_StatementList *body = parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        AST_FunctionDefinition *function_definition =
            new AST_FunctionDefinition(token_function.location, name,
                                       parameter_list, return_type, body);

        // NOTE: After we parse a function definition, we do all the interesting
        // stuff!
        // function_definition->print(std::cout, symbol_table, false, {});
        // symbol_table->print(std::cout);
        type_checker.type_check(function_definition);
        quads.generate_quads(function_definition);
        code_generator.generate_code(quads);

        symbol_table->close_scope();

        return function_definition;
    }
    case Token::Kind::If:
    {
        Token token_if = tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        AST_Expression *condition = parse_expression();

        expect(Token::Kind::RightParentheses);

        expect(Token::Kind::LeftCurlyBrace);

        AST_StatementList *body = parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        return new AST_If(token_if.location, condition, body);
    }
    case Token::Kind::For:
    {
        report_parse_error(tokenizer.peek(1).location, "For not implemented");

        /*
        tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        parse_expression();

        expect(Token::Kind::RightParentheses);

        expect(Token::Kind::LeftCurlyBrace);

        parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        */
        // TODO: Return proper value
        return nullptr;
    }
    // TODO: Add another case here that gives the possibility of adding scopes,
    // ie only '{' and '}' instead of functions. This will only open/close
    // scope instead of creating a function
    default:
    {
        report_parse_error_unexpected_token(tokenizer.peek(1));
        return nullptr;
    }
    }
}

AST_Identifier *Parser::parse_optional_return()
{
    if (tokenizer.peek(1).kind == Token::Kind::Arrow)
    {
        tokenizer.eat(); // Eat '->'

        Token ident = expect(Token::Kind::Identifier);

        int type_index = symbol_table->lookup_symbol(ident.text);

        if (type_index == -1)
        {
            report_parse_error_undefined_reference(ident);
            return nullptr;
        }

        Symbol *type_symbol = symbol_table->get_symbol(type_index);

        if (type_symbol->tag != Symbol::Tag::Type)
        {
            report_parse_error(ident.location,
                               "Symbol '" + ident.text + "' is not a type");
            return nullptr;
        }

        return new AST_Identifier(ident.location, type_index);
    }
    else
    {
        return nullptr;
    }
}

AST_ExpressionList *Parser::parse_optional_argument_list()
{
    // TODO: This is the same ugly hack as we did above
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Minus:
    case Token::Kind::Integer:
    case Token::Kind::Real:
    case Token::Kind::True:
    case Token::Kind::False:
    case Token::Kind::Identifier:
    case Token::Kind::LeftParentheses:
    {
        return parse_argument_list();
    }
    default:
    {
        return nullptr;
    }
    }
}

AST_ExpressionList *Parser::parse_argument_list()
{
    AST_Expression *argument = parse_expression();
    ASSERT(argument != nullptr);

    AST_ExpressionList *argument_list;
    if (tokenizer.peek(1).kind == Token::Kind::Comma)
    {
        tokenizer.eat();

        argument_list = parse_argument_list();
    }
    else
    {
        argument_list = nullptr;
    }

    return new AST_ExpressionList(argument->location, argument, argument_list);
}

AST_ParameterList *Parser::parse_optional_parameter_list()
{
    // TODO: Same hack for the third time
    if (tokenizer.peek(1).kind == Token::Kind::Identifier)
    {
        return parse_parameter_list();
    }
    else
    {
        return nullptr;
    }
}

AST_ParameterList *Parser::parse_parameter_list()
{
    AST_Identifier *parameter = parse_parameter();
    ASSERT(parameter != nullptr);

    AST_ParameterList *param_list;

    if (tokenizer.peek(1).kind == Token::Kind::Comma)
    {
        tokenizer.eat();

        param_list = parse_parameter_list();
    }
    else
    {
        param_list = nullptr;
    }

    return new AST_ParameterList(parameter->location, parameter, param_list);
}

AST_Identifier *Parser::parse_parameter()
{
    Token name = expect(Token::Kind::Identifier);

    expect(Token::Kind::Colon);

    Token type = expect(Token::Kind::Identifier);

    int type_index = symbol_table->lookup_symbol(type.text);

    Symbol *type_symbol = symbol_table->get_symbol(type_index);

    if (type_index == -1)
    {
        report_parse_error_undefined_reference(type);
    }
    else if (type_symbol->tag != Symbol::Tag::Type)
    {
        report_parse_error(type.location,
                           "Symbol '" + type.text + "' is not a type");
    }

    int symbol_index =
        symbol_table->insert_parameter(name.location, name.text, type_index);

    return new AST_Identifier(name.location, symbol_index);
}

AST_Expression *Parser::parse_expression()
{
    AST_Expression *lhs = parse_minus_term();

    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Plus:
    {
        tokenizer.eat();
        return respect_precedence(
            new AST_Plus(lhs->location, lhs, parse_expression()));
    }
    case Token::Kind::Minus:
    {
        tokenizer.eat();
        return respect_precedence(
            new AST_Minus(lhs->location, lhs, parse_expression()));
    }
    case Token::Kind::Multiplication:
    {
        tokenizer.eat();
        return respect_precedence(
            new AST_Multiplication(lhs->location, lhs, parse_expression()));
    }
    case Token::Kind::Division:
    {
        tokenizer.eat();
        return respect_precedence(
            new AST_Division(lhs->location, lhs, parse_expression()));
    }
    case Token::Kind::LesserThan:
    {
        tokenizer.eat();

        return new AST_LesserThan(lhs->location, lhs, parse_expression());
    }
    case Token::Kind::LesserThanOrEqual:
    {
        tokenizer.eat();

        return new AST_LesserThanOrEqual(lhs->location, lhs,
                                         parse_expression());
    }
    case Token::Kind::DoubleEquals:
    {
        tokenizer.eat();

        return new AST_DoubleEquals(lhs->location, lhs, parse_expression());
    }
    case Token::Kind::GreaterThan:
    {
        tokenizer.eat();

        return new AST_GreaterThan(lhs->location, lhs, parse_expression());
    }

    case Token::Kind::GreaterThanOrEqual:
    {
        tokenizer.eat();

        return new AST_GreaterThanOrEquals(lhs->location, lhs,
                                           parse_expression());
    }
    default:
    {
        return lhs;
    }
    }
}

AST_Expression *Parser::parse_optional_expression()
{
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Identifier:
    case Token::Kind::Integer:
    case Token::Kind::Real:
    case Token::Kind::True:
    case Token::Kind::False:
    case Token::Kind::Minus:
    case Token::Kind::LeftParentheses:
    {
        return parse_expression();
    }
    default:
    {
        return nullptr;
    }
    }
}

AST_Expression *Parser::parse_minus_term()
{
    if (tokenizer.peek(1).kind == Token::Kind::Minus)
    {
        Token token_minus = tokenizer.eat();
        return new AST_UnaryMinus(token_minus.location, parse_term());
    }
    else
    {
        return parse_term();
    }
}

AST_Expression *Parser::parse_term()
{
    if (tokenizer.peek(1).kind == Token::Kind::Integer)
    {
        Token token_integer = tokenizer.eat();
        return new AST_Integer(token_integer.location,
                               token_integer.integer_value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Real)
    {
        Token token_real = tokenizer.eat();
        return new AST_Real(token_real.location, token_real.real_value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::True)
    {
        Token token_true = tokenizer.eat();
        return new AST_Bool(token_true.location, token_true.integer_value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::False)
    {
        Token token_false = tokenizer.eat();
        return new AST_Bool(token_false.location, token_false.integer_value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Identifier)
    {
        if (tokenizer.peek(2).kind == Token::Kind::LeftParentheses)
        {
            return parse_function_call();
        }

        // NOTE: We assume it was just an identifier
        else
        {
            Token name = tokenizer.eat();

            int symbol_index = symbol_table->lookup_symbol(name.text);

            if (symbol_index == -1)
            {
                report_parse_error_undefined_reference(name);
            }

            AST_Identifier *ident =
                new AST_Identifier(name.location, symbol_index);

            Symbol *symbol = symbol_table->get_symbol(symbol_index);

            if (symbol->tag != Symbol::Tag::Variable &&
                symbol->tag != Symbol::Tag::Parameter)
            {
                report_parse_error(name.location,
                                   "Symbol '" + symbol->name +
                                       "' is not a variable or parameter");
            }

            return ident;
        }
    }
    else if (tokenizer.peek(1).kind == Token::Kind::LeftParentheses)
    {
        expect(Token::Kind::LeftParentheses);
        AST_Expression *expr = parse_expression();
        expect(Token::Kind::RightParentheses);
        return expr;
    }
    else
    {
        report_parse_error_unexpected_token(tokenizer.peek(1));
        return nullptr;
    }
}

AST_FunctionCall *Parser::parse_function_call()
{
    Token tok_name = expect(Token::Kind::Identifier);

    expect(Token::Kind::LeftParentheses);

    AST_ExpressionList *arguments = parse_optional_argument_list();

    expect(Token::Kind::RightParentheses);

    std::string type_string = "";
    if (arguments != nullptr)
    {
        type_string = arguments->get_type_string(&type_checker);
    }

    std::string name = tok_name.text + type_string;

    int symbol_index = symbol_table->lookup_symbol(name);

    if (symbol_index == -1)
    {
        std::string args{""};
        if (arguments != nullptr)
        {
            args = arguments->get_debug_type_string(&type_checker);
        }

        report_parse_error(tok_name.location,
                           "No function found matching call signature '" +
                               tok_name.text + "(" + args + ")'");
    }

    AST_Identifier *ident = new AST_Identifier(tok_name.location, symbol_index);

    Symbol *symbol = symbol_table->get_symbol(symbol_index);

    if (symbol->tag != Symbol::Tag::Function)
    {
        report_parse_error(symbol->location,
                           "Symbol '" + symbol->name + "' is not a function");
    }

    return new AST_FunctionCall(tok_name.location, ident, arguments);
}
