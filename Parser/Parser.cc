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

AST_Node *Parser::parse_statement_list()
{
    parse_statement();
    parse_statement_list_tail();
    return nullptr;
}

AST_Node *Parser::parse_statement_list_tail()
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

AST_Node *Parser::parse_statement()
{
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Identifier:
    {
        tokenizer.eat();

        parse_statement_tail();

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::Return:
    {
        tokenizer.eat();
        // TODO: Return proper value

        parse_optional_argument_list();

        return nullptr;
    }
    case Token::Kind::Function:
    {
        tokenizer.eat();

        // TODO: Handle identifier
        tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        parse_optional_parameter_list();

        expect(Token::Kind::RightParentheses);

        parse_optional_return();

        expect(Token::Kind::LeftCurlyBrace);

        // TODO: Should we allow empty functions bodies?
        parse_statement_list();

        expect(Token::Kind::RightCurlyBrace);

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::If:
    {
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
    case Token::Kind::While:
    {
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

AST_Node *Parser::parse_statement_tail()
{
    switch (tokenizer.peek(1).kind)
    {
    case Token::Kind::Colon:
    {
        tokenizer.eat();

        // TODO: Handle identifier
        tokenizer.eat();

        expect(Token::Kind::Equals);

        parse_expression();

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::Equals:
    {
        tokenizer.eat();

        parse_expression();

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::LeftParentheses:
    {
        tokenizer.eat();

        parse_optional_argument_list();

        expect(Token::Kind::RightParentheses);

        // TODO: Return proper value
        return nullptr;
    }
    default:
    {
        report_syntax_error_and_stop("Failed to parse statement tail");
        return nullptr;
    }
    }
}

AST_Node *Parser::parse_optional_return()
{
    if (tokenizer.peek(1).kind == Token::Kind::Arrow)
    {
        tokenizer.eat();

        expect(Token::Kind::LeftParentheses);

        parse_parameter_list();

        expect(Token::Kind::RightParentheses);

        // TODO: Return proper value
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

AST_Node *Parser::parse_optional_argument_list()
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
        parse_argument_list();

        // TODO: Return proper value
        return nullptr;
    }
    default:
    {
        return nullptr;
    }
    }
}

AST_Node *Parser::parse_argument_list()
{
    parse_argument();
    parse_argument_list_tail();

    // TODO: Return proper value
    return nullptr;
}

AST_Node *Parser::parse_argument_list_tail()
{
    if (tokenizer.peek(1).kind == Token::Kind::Comma)
    {
        tokenizer.eat();

        parse_argument_list();

        // TODO: Return proper value
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

AST_Node *Parser::parse_argument() { return parse_expression(); }

AST_Node *Parser::parse_optional_parameter_list()
{
    // TODO: Same hack for the third time
    if (tokenizer.peek(1).kind == Token::Kind::Identifier)
    {
        parse_parameter_list();

        // TODO: Return proper value
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

AST_Node *Parser::parse_parameter_list()
{
    parse_parameter();
    parse_parameter_list_tail();

    // TODO: Return proper value
    return nullptr;
}

AST_Node *Parser::parse_parameter_list_tail()
{
    if (tokenizer.peek(1).kind == Token::Kind::Comma)
    {
        tokenizer.eat();
        parse_parameter_list();

        // TODO: Return proper value
        return nullptr;
    }
    else
    {
        return nullptr;
    }
}

AST_Node *Parser::parse_parameter()
{
    // TODO: Handle identifier
    tokenizer.eat();

    expect(Token::Kind::Colon);

    // TODO: Handle identifier
    tokenizer.eat();

    // TODO: Return proper value
    return nullptr;
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
        tokenizer.eat();

        parse_expression();

        // TODO: Return proper value
        return nullptr;
    }
    case Token::Kind::GreaterThan:
    {
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

        Symbol *symbol = new Symbol(ident_text, 1);

        AST_Identifier *ident = new AST_Identifier(symbol, ident_text);
        AST_Expression *args  = parse_term_tail();

        if (args)
        {
            return new AST_FunctionCall(symbol, ident, args);
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

AST_Expression *Parser::parse_term_tail()
{
    if (tokenizer.peek(1).kind == Token::Kind::LeftParentheses)
    {
        expect(Token::Kind::LeftParentheses);

        // TODO: Insted use:   parse_optional_argument_list() and return proper
        // value
        AST_Expression *expr = parse_expression();

        expect(Token::Kind::RightParentheses);

        if (expr)
        {
            return expr;
        }
        else
        {
            report_syntax_error_and_stop("Failed to parse term tail");
            return nullptr;
        }
    }
    else
    {
        return nullptr;
    }
}
