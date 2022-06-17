#include "Parser.h"
#include "AST/AST.h"
#include "Tokenizer/Tokenizer.h"
#include <iostream>
#include <sstream>
#include <stdlib.h>
#include <string>

Parser::Parser(std::istream &is) : tokenizer{Tokenizer{is}}
{
    tokenizer.tokenize();
}

Parser::Parser(Tokenizer &tokenizer) : tokenizer{tokenizer} {}

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
              << tokenizer.peek(1).begin_column + 1 << ": " << message
              << std::endl;
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

AST_Expression *Parser::parse_start() { return parse_expression(); }

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
    if (tokenizer.peek(1).kind == Token::Kind::Plus)
    {
        tokenizer.eat();
        return new AST_Plus(nullptr, parse_expression());
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Minus)
    {
        tokenizer.eat();
        return new AST_Minus(nullptr, parse_expression());
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Multiplication)
    {
        tokenizer.eat();
        return new AST_Multiplication(nullptr, parse_expression());
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Division)
    {
        tokenizer.eat();
        return new AST_Division(nullptr, parse_expression());
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Exponentiation)
    {
        tokenizer.eat();
        return new AST_Exponentiation(nullptr, parse_expression());
    }
    else
    {
        return nullptr;
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
    if (tokenizer.peek(1).kind == Token::Kind::Number)
    {
        return new AST_Number(tokenizer.eat().value);
    }
    else if (tokenizer.peek(1).kind == Token::Kind::Identifier)
    {
        AST_Identifier *ident = new AST_Identifier(tokenizer.eat().text);
        AST_Expression *args  = parse_term_tail();

        if (args)
        {
            return new AST_FunctionCall(ident, args);
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
