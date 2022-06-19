#include "Parser.h"
#include "AST/AST.h"
#include "Tokenizer/Tokenizer.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

Parser::Parser(std::istream &is)
    : tokenizer{Tokenizer{is}}, symbolTable{new SymbolTable()}
{
    tokenizer.tokenize();
}

Parser::Parser(Tokenizer &tokenizer, SymbolTable *symbolTable)
    : tokenizer{tokenizer}, symbolTable{symbolTable}
{}

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
        oss << "Unexpected token: '" << next.text << "', "
            << "expected <" << kind << ">, got <" << tokenizer.peek(1).kind
            << ">";
        report_syntax_error_and_stop(oss.str());
        std::exit(1);
    }
}

void Parser::report_syntax_error_and_stop(std::string message)
{
    std::cout << "SyntaxError:" << tokenizer.peek(1).line + 1 << ","
              << tokenizer.peek(1).begin_column << ": " << message << std::endl;
    std::exit(1);
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

    return expr;
}

AST_Node *Parser::parse_start() { return parse_statement_list(); }

AST_StatementList *Parser::parse_statement_list()
{
    AST_Statement     *statement      = parse_statement();
    AST_StatementList *statement_list = parse_statement_list_tail();

    if (statement_list != nullptr)
    {
        statement_list->add_statement(statement);
        return statement_list;
    }
    else
    {
        return new AST_StatementList(statement);
    }
}

AST_StatementList *Parser::parse_statement_list_tail()
{
    // TODO: Do this a better way. Statement assumes that it will always succeed
    // when parsing but here its fine if it fails. So we need to check here if
    // its going to succeed, which is kinda ugly.

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
        Token identifier = tokenizer.eat();

        AST_Statement *statement = parse_statement_tail();

        // NOTE: Here we don't know if we just parsed a defintion, an assignment
        // or a function call, so we need to check that

        AST_VariableAssignment *assignment =
            dynamic_cast<AST_VariableAssignment *>(statement);
        if (assignment != nullptr)
        {
            // TODO: Check that the variable is already defined, and create a
            // new identfier with that symbol
            assignment->lhs = new AST_Identifier(nullptr, identifier.text);
            return assignment;
        }

        AST_VariableDefinition *defintion =
            dynamic_cast<AST_VariableDefinition *>(statement);
        if (defintion != nullptr)
        {
            defintion->lhs->symbol->name = identifier.text;
            defintion->lhs->name         = identifier.text;
            return defintion;
        }

        AST_FunctionCall *function_call =
            dynamic_cast<AST_FunctionCall *>(statement);
        if (function_call != nullptr)
        {
            // TODO: Find the symbol with the name identifier.text and use it to
            // create a new identifier for the function call
            function_call->ident = new AST_Identifier(nullptr, identifier.text);
            return function_call;
        }

        report_syntax_error_and_stop(
            "Internal compiler error: Failed to parse Statement");
        return nullptr;
    }
    case Token::Kind::Return:
    {
        tokenizer.eat();

        AST_ExpressionList *return_values = parse_optional_argument_list();

        return new AST_Return(return_values);
    }
    case Token::Kind::Function:
    {
        tokenizer.eat();

        Token   function_token  = expect(Token::Kind::Identifier);
        Symbol *function_symbol = new Symbol(function_token.text, "Unknown");
        symbolTable->add_symbol(function_symbol);

        AST_Identifier *name =
            new AST_Identifier(function_symbol, function_token.text);

        expect(Token::Kind::LeftParentheses);

        AST_ParameterList *parameter_list = parse_optional_parameter_list();

        expect(Token::Kind::RightParentheses);

        AST_ParameterList *return_values = parse_optional_return();

        expect(Token::Kind::LeftCurlyBrace);

        // TODO: Should empty function bodies be allowed?
        AST_StatementList *body = parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        return new AST_FunctionDefinition(name, parameter_list, return_values,
                                          body);
    }
    case Token::Kind::If:
    {
        tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        AST_Expression *condition = parse_expression();

        expect(Token::Kind::RightParentheses);

        expect(Token::Kind::LeftCurlyBrace);

        AST_StatementList *body = parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        return new AST_If(condition, body);
    }
    case Token::Kind::While:
    {
        report_syntax_error_and_stop("Not implemented: While");
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
    default:
    {
        report_syntax_error_and_stop("Failed to parse statement");
        return nullptr;
    }
    }
}

AST_Statement *Parser::parse_statement_tail()
{
    switch (tokenizer.peek(1).kind)
    {
    // Definition
    case Token::Kind::Colon:
    {
        tokenizer.eat();

        Token   token_type = expect(Token::Kind::Identifier);
        Symbol *symbol     = new Symbol("", token_type.text);
        symbolTable->add_symbol(symbol);
        AST_Identifier *identifier =
            new AST_Identifier(symbol, token_type.text);

        expect(Token::Kind::Equals);

        AST_Expression *expression = parse_expression();

        return new AST_VariableDefinition(identifier, expression);
    }
    // Assignment
    case Token::Kind::Equals:
    {
        tokenizer.eat();

        AST_Expression *expression = parse_expression();

        return new AST_VariableAssignment(nullptr, expression);
    }
    // Function call
    case Token::Kind::LeftParentheses:
    {
        tokenizer.eat();

        AST_ExpressionList *arguments = parse_optional_argument_list();

        expect(Token::Kind::RightParentheses);

        return new AST_FunctionCall(nullptr, arguments);
    }
    default:
    {
        report_syntax_error_and_stop("Failed to parse statement tail");
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
    AST_Expression     *argument      = parse_argument();
    AST_ExpressionList *argument_list = parse_argument_list_tail();

    if (argument_list != nullptr)
    {
        argument_list->add_expression(argument);
        return argument_list;
    }
    else
    {
        return new AST_ExpressionList(argument);
    }
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
    AST_Identifier    *parameter  = parse_parameter();
    AST_ParameterList *param_list = parse_parameter_list_tail();

    // When we have reached the end of the parameters,
    // parse_parameter_list_tail() returns nullptr. Then we create the
    // parameterlist with the last parameter and send it back up and populate it
    // with the already parsed parameters.
    if (param_list != nullptr)
    {
        param_list->add_parameter(parameter);
        return param_list;
    }
    else
    {
        return new AST_ParameterList(parameter);
    }
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

    Symbol *symbol = new Symbol(name.text, type.text);
    symbolTable->add_symbol(symbol);

    return new AST_Identifier(symbol, name.text);
}

AST_Expression *Parser::parse_expression()
{
    AST_Expression      *expr  = parse_minus_term();
    AST_BinaryOperation *binop = parse_expression_tail();

    if (binop)
    {
        binop->lhs = expr;
        return respect_precedence(binop);
    }
    else
    {
        return expr;
    }
}

AST_BinaryOperation *Parser::parse_expression_tail()
{

    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Plus:
    {
        tokenizer.eat();
        return new AST_Plus(nullptr, parse_expression());
    }
    case Token::Kind::Minus:
    {
        tokenizer.eat();
        return new AST_Minus(nullptr, parse_expression());
    }
    case Token::Kind::Multiplication:
    {
        tokenizer.eat();
        return new AST_Multiplication(nullptr, parse_expression());
    }
    case Token::Kind::Division:
    {
        tokenizer.eat();
        return new AST_Division(nullptr, parse_expression());
    }
    case Token::Kind::LesserThan:
    {
        report_syntax_error_and_stop("Not implemented: Less than");
        tokenizer.eat();

        parse_expression();

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::GreaterThan:
    {
        report_syntax_error_and_stop("Not implemented: Greater than");
        tokenizer.eat();

        parse_expression();

        // TODO: Return proper value
        return nullptr;
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
        tokenizer.eat();
        return new AST_UnaryMinus(parse_term());
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
        return new AST_Integer(tokenizer.eat().integer_value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Real)
    {
        return new AST_Real(tokenizer.eat().real_value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Identifier)
    {
        std::string ident_text = tokenizer.eat().text;

        Symbol *symbol = new Symbol(ident_text, "Unknown");

        AST_Identifier     *ident     = new AST_Identifier(symbol, ident_text);
        AST_ExpressionList *arguments = parse_term_tail();

        if (arguments)
        {
            return new AST_FunctionCall(ident, arguments);
        }
        else
        {
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
        report_syntax_error_and_stop("Failed to parse term");
        return nullptr;
    }
}

AST_ExpressionList *Parser::parse_term_tail()
{
    if (tokenizer.peek(1).kind == Token::Kind::LeftParentheses)
    {
        expect(Token::Kind::LeftParentheses);

        AST_ExpressionList *arguments = parse_optional_argument_list();

        expect(Token::Kind::RightParentheses);

        // if (arguments)
        // {
        return arguments;
        // }
        /*
        else
        {
            report_syntax_error_and_stop("Failed to parse term tail");
            return nullptr;
        }
        */
    }
    else
    {
        return nullptr;
    }
}
