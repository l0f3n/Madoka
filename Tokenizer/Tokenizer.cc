#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN

#include "Tokenizer.h"
#include "Token.h"
#include <iostream>
#include <string>

Tokenizer::Tokenizer(std::istream &is)
    : is{is}, line{0}, column{0}, begin_column{0}
{}

void Tokenizer::tokenize()
{
    Token token;
    do
    {
        token = tokenize_next_token();
        tokens.push_back(token);
    } while (token.kind != Token::Kind::End);
}

void Tokenizer::print(std::ostream &os)
{
    for (Token token : tokens)
    {
        os << token << std::endl;
    }
}

char Tokenizer::peek_next_char() { return is.peek(); }

char Tokenizer::consume_next_char()
{
    column++;
    return is.get();
}

void Tokenizer::consume_whitespace()
{
    while (true)
    {
        switch (peek_next_char())
        {
        case ' ':
        case '\t':
        case '\r': consume_next_char(); break;
        case '\n':
            consume_next_char();
            line++;
            column = 0;
            break;
        default: return;
        }
    }
}

bool Tokenizer::is_digit(char c)
{
    switch (c)
    {
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9': return true;
    default: return false;
    }
}

bool Tokenizer::is_identifier_char(char c)
{
    switch (c)
    {
    case 'a':
    case 'b':
    case 'c':
    case 'd':
    case 'e':
    case 'f':
    case 'g':
    case 'h':
    case 'i':
    case 'j':
    case 'k':
    case 'l':
    case 'm':
    case 'n':
    case 'o':
    case 'p':
    case 'q':
    case 'r':
    case 's':
    case 't':
    case 'u':
    case 'v':
    case 'w':
    case 'x':
    case 'y':
    case 'z':
    case 'A':
    case 'B':
    case 'C':
    case 'D':
    case 'E':
    case 'F':
    case 'G':
    case 'H':
    case 'I':
    case 'J':
    case 'K':
    case 'L':
    case 'M':
    case 'N':
    case 'O':
    case 'P':
    case 'Q':
    case 'R':
    case 'S':
    case 'T':
    case 'U':
    case 'V':
    case 'W':
    case 'X':
    case 'Y':
    case 'Z':
    case '0':
    case '1':
    case '2':
    case '3':
    case '4':
    case '5':
    case '6':
    case '7':
    case '8':
    case '9':
    case '_': return true;
    default: return false;
    }
}

Token Tokenizer::create_token(Token::Kind kind)
{
    return Token{kind, line, begin_column, column - 1};
}

Token Tokenizer::create_token(Token::Kind kind, std::string &text)
{
    return Token{kind, line, begin_column, column - 1, text};
}

Token Tokenizer::create_token(Token::Kind kind, std::string &text, double value)
{
    return Token{kind, line, begin_column, column - 1, text, value};
}

Token Tokenizer::tokenize_next_token()
{
    consume_whitespace();

    begin_column = column;

    switch (peek_next_char())
    {
    case EOF: column += 1; return create_token(Token::Kind::End);
    case '(':
    {
        consume_next_char();
        return create_token(Token::Kind::LeftParentheses);
    }
    case ')':
    {
        consume_next_char();
        return create_token(Token::Kind::RightParentheses);
    }
    case '=':
    {
        consume_next_char();
        return create_token(Token::Kind::Equals);
    }
    case '+':
    {
        consume_next_char();
        return create_token(Token::Kind::Plus);
    }
    case '-':
    {
        consume_next_char();
        return create_token(Token::Kind::Minus);
    }
    case '*':
    {
        consume_next_char();
        return create_token(Token::Kind::Multiplication);
    }
    case '/':
    {
        consume_next_char();
        return create_token(Token::Kind::Division);
    }
    case '^':
    {
        consume_next_char();
        return create_token(Token::Kind::Exponentiation);
    }
    }

    if (is_digit(peek_next_char()))
    {
        std::string text;
        while (is_digit(peek_next_char()))
            text.push_back(consume_next_char());

        if (peek_next_char() == '.')
        {
            text.push_back(consume_next_char());
            while (is_digit(peek_next_char()))
                text.push_back(consume_next_char());

            return create_token(Token::Kind::Number, text, std::stof(text));
        }
        else
        {
            return create_token(Token::Kind::Number, text, std::stof(text));
        }
    }
    else if (is_identifier_char(peek_next_char()))
    {
        std::string text;
        while (is_identifier_char(peek_next_char()))
            text.push_back(consume_next_char());

        return create_token(Token::Kind::Identifier, text);
    }

    std::string s(1, consume_next_char());
    return create_token(Token::Kind::Unexpected, s);
}

Token Tokenizer::peek() { return peek(1); }

Token Tokenizer::peek(int n) { return tokens[token_index + n]; }

Token Tokenizer::eat() { return tokens[++token_index]; }
