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
        std::ostringstream oss{};
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

    AST_Node *expr = parse_start();

    expect(Token::Kind::End);

    // TODO: I don't like that we have to call code generation functions
    // directly from here, it doesn't seem very good to me

    // NOTE: We have a function called '#global' that runs automatically
    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    // NOTE: This feels really weird. Manually generating an AST for a function
    // called '#global' and making it call 'main'. Think about doing it some
    // other way

    Location no_location{-1, -1, -1, -1};

    // TODO: Probably create constant to refer to #global
    AST_Identifier *name =
        new AST_Identifier(no_location, symbol_table->lookup_symbol("#global"));

    AST_ParameterList *parameters = nullptr;

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

    AST_Identifier *main =
        new AST_Identifier(no_location, symbol_table->lookup_symbol("main"));

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

AST_Node *Parser::parse_start() { return parse_statement_list(); }

AST_StatementList *Parser::parse_statement_list()
{
    AST_Statement *statement = parse_statement();
    ASSERT(statement != nullptr);

    // TODO: This is weird, but works for now
    AST_FunctionDefinition *function_definition =
        dynamic_cast<AST_FunctionDefinition *>(statement);

    // NOTE: After we have parsed a function definition, we typecheck it. This
    // also include typechecking its entire body. So everything that is not
    // global will be included by doing this every time we have parsed a
    // function
    if (function_definition != nullptr)
    {
        // TODO: Make this function call look cleaner, add another overload
        // probably
        // function_definition->print(std::cout, symbol_table, true, {});
        // std::cout << std::endl;

        type_checker.type_check(function_definition);

        quads.generate_quads(function_definition);

        code_generator.generate_code(quads);

        // NOTE: We opened the scope when we parsed the function below but we
        // cant close it their since we need to lookup the symbols from that
        // scope when typechecking, generating quads and generating code. So we
        // close it here instead.
        symbol_table->close_scope();
    }

    AST_StatementList *statement_list = parse_statement_list_tail();
    return new AST_StatementList(statement->location, statement,
                                 statement_list);
}

AST_StatementList *Parser::parse_statement_list_tail()
{
    // TODO: Do this a better way. Statement assumes that it will always succeed
    // when parsing but here its fine if it fails. So we need to check here if
    // its going to succeed, which is kinda ugly.
    //
    // Is it unreasonable to do this iteratively instead? Is it possible? I feel
    // like the code would look better that way.

    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Identifier:
    case Token::Kind::Return:
    case Token::Kind::Function:
    case Token::Kind::LeftCurlyBrace:
    case Token::Kind::If:
    case Token::Kind::While:
    {
        return parse_statement_list();
    }
    default:
    {
        return nullptr;
    }
    }
}

AST_Statement *Parser::parse_statement()
{
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Identifier:
    {
        Token name_token = tokenizer.eat();
        switch (tokenizer.peek(1).kind)
        {
        // Definition
        case Token::Kind::Colon:
        {
            tokenizer.eat();

            Token type_token = expect(Token::Kind::Identifier);

            expect(Token::Kind::Equals);

            AST_Expression *expression = parse_expression();

            // NOTE: We are done parsing the definition, now we can do
            // all checks for correctness

            int type_index = symbol_table->lookup_symbol(type_token.text);

            if (type_index == -1)
            {
                error(type_token.location)
                    << "Unknown type '" << type_token.text << "'" << std::endl;
                std::exit(1);
            }
            else if (symbol_table->get_symbol(type_index)->tag !=
                     Symbol::Tag::Type)
            {
                error(type_token.location) << "Symbol '" << type_token.text
                                           << "' is not a type" << std::endl;
                std::exit(1);
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
            tokenizer.eat();

            AST_ExpressionList *arguments = parse_optional_argument_list();

            expect(Token::Kind::RightParentheses);

            int symbol_index = symbol_table->lookup_symbol(name_token.text);

            if (symbol_index == -1)
            {
                report_parse_error_undefined_reference(name_token);
            }
            else if (symbol_table->get_symbol(symbol_index)->tag !=
                     Symbol::Tag::Function)
            {
                error(name_token.location)
                    << "Symbol '" << name_token.text << "' is not a function"
                    << std::endl;
                std::exit(1);
            }

            AST_Identifier *name =
                new AST_Identifier(name_token.location, symbol_index);

            return new AST_FunctionCall(name_token.location, name, arguments);
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

        AST_ExpressionList *return_values = parse_optional_argument_list();

        // TODO: We need to handle the case where there are multiple branching
        // paths in the body, and check if every path has a return. But we
        // haven't implemented If statements yet.
        FunctionSymbol *function =
            symbol_table->get_function_symbol(symbol_table->enclosing_scope());
        function->has_return = true;

        return new AST_Return(token_return.location, return_values);
    }
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

        // TODO: We can't parse the return values the same way as we parse
        // parameters, since the will get added to the parameters of the
        // function. This is really broken right now.
        AST_ParameterList *return_values = parse_optional_return();

        expect(Token::Kind::LeftCurlyBrace);

        // TODO: Should empty function bodies be allowed?
        AST_StatementList *body = parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        // TODO: Instead of returning immediately, we should probably do
        // typechecking, quad and code generation here here, and then return
        return new AST_FunctionDefinition(token_function.location, name,
                                          parameter_list, return_values, body);
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
    case Token::Kind::While:
    {
        report_parse_error(tokenizer.peek(1).location, "While not implemented");

        tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        parse_expression();

        expect(Token::Kind::RightParentheses);

        expect(Token::Kind::LeftCurlyBrace);

        parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

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

AST_ParameterList *Parser::parse_optional_return()
{
    if (tokenizer.peek(1).kind == Token::Kind::Arrow)
    {
        tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        AST_ParameterList *return_values = parse_parameter_list();

        expect(Token::Kind::RightParentheses);

        return return_values;
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
    AST_Expression *argument = parse_argument();
    ASSERT(argument != nullptr);

    AST_ExpressionList *argument_list = parse_argument_list_tail();

    return new AST_ExpressionList(argument->location, argument, argument_list);
}

AST_ExpressionList *Parser::parse_argument_list_tail()
{
    if (tokenizer.peek(1).kind == Token::Kind::Comma)
    {
        tokenizer.eat();

        return parse_argument_list();
    }
    else
    {
        return nullptr;
    }
}

AST_Expression *Parser::parse_argument() { return parse_expression(); }

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
    // TODO: We need to split these into parse_parameter_list() and
    // parse_return_value_list() to be able to handle them differently

    AST_Identifier *parameter = parse_parameter();
    ASSERT(parameter != nullptr);

    AST_ParameterList *param_list = parse_parameter_list_tail();

    return new AST_ParameterList(parameter->location, parameter, param_list);

    // NOTE: When we have reached the end of the parameters,
    // parse_parameter_list_tail() returns nullptr. Then we create the
    // parameterlist with the last parameter and send it back up and populate it
    // with the already parsed parameters.
    /*
    if (param_list != nullptr)
    {
        param_list->add_parameter(parameter);
        return param_list;
    }
    else
    {
        // TODO: Same as the other two
        return new AST_ParameterList(parameter->location, parameter, nullptr);
    }
    */
}

AST_ParameterList *Parser::parse_parameter_list_tail()
{
    if (tokenizer.peek(1).kind == Token::Kind::Comma)
    {
        tokenizer.eat();
        return parse_parameter_list();
    }
    else
    {
        return nullptr;
    }
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
        error(type.location)
            << "Symbol '" << type.text << "' is not a type" << std::endl;
        std::exit(1);
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
        report_parse_error(tokenizer.peek(1).location,
                           "Lesser than not implemented");
        tokenizer.eat();

        parse_expression();

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::GreaterThan:
    {
        report_parse_error(tokenizer.peek(1).location,
                           "Greater than not implemented");
        tokenizer.eat();

        parse_expression();

        // TODO: Return proper value
        return nullptr;
    }
    default:
    {
        return lhs;
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
    else if (tokenizer.peek(1).kind == Token::Kind::Identifier)
    {
        Token name = tokenizer.eat();

        int symbol_index = symbol_table->lookup_symbol(name.text);

        if (symbol_index == -1)
        {
            report_parse_error_undefined_reference(name);
        }

        AST_Identifier *ident = new AST_Identifier(name.location, symbol_index);

        // NOTE: If we find a left parentheses here, we try to parse a function
        if (tokenizer.peek(1).kind == Token::Kind::LeftParentheses)
        {
            expect(Token::Kind::LeftParentheses);

            AST_ExpressionList *arguments = parse_optional_argument_list();

            expect(Token::Kind::RightParentheses);

            Symbol *symbol = symbol_table->get_symbol(symbol_index);

            if (symbol->tag != Symbol::Tag::Function)
            {
                error(symbol->location) << "Symbol '" << symbol->name
                                        << "' is not a function" << std::endl
                                        << std::endl;
                std::exit(1);
            }

            return new AST_FunctionCall(name.location, ident, arguments);
        }
        // NOTE: Otherwise, we assume it was just an identifier
        else
        {
            Symbol *symbol = symbol_table->get_symbol(symbol_index);

            if (symbol->tag != Symbol::Tag::Variable &&
                symbol->tag != Symbol::Tag::Parameter)
            {
                error(name.location)
                    << "Symbol '" << symbol->name
                    << "' is not a variable or parameter" << std::endl
                    << std::endl;
                std::exit(1);
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
