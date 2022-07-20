#include "Tokenizer.h"
#include "Token.h"
#include <iostream>
#include <string>

Tokenizer::Tokenizer(std::istream &is) : is{is} {}

void Tokenizer::tokenize()
{
    Token token;
    do
    {
        token = tokenize_next_token();
        if (!inline_comment && !multiline_comment &&
            token.kind != Token::Kind::DoubleSemicolon)
        {
            tokens.push_back(token);
        }

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
        case '\r':
        {
            consume_next_char();
            break;
        }
        case '\n':
        {
            consume_next_char();
            inline_comment = false;
            line++;
            column = 0;
            break;
        }

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

// TODO: These constructors look ugly, cleanup
Token Tokenizer::create_token(Token::Kind kind)
{
    return Token{kind, line, line, begin_column, column - 1, "", (long)0};
}

// TODO: These constructors look ugly, cleanup
Token Tokenizer::create_token(Token::Kind kind, std::string text)
{
    return Token{kind, line, line, begin_column, column - 1, text, (long)0};
}

Token Tokenizer::create_token(Token::Kind kind, std::string text, long value)
{
    return Token{kind, line, line, begin_column, column - 1, text, value};
}

Token Tokenizer::create_token(Token::Kind kind, std::string text, double value)
{
    return Token{kind, line, line, begin_column, column - 1, text, value};
}

Token Tokenizer::tokenize_next_token()
{
    consume_whitespace();

    begin_column = column;

    switch (peek_next_char())
    {
    case EOF:
    {
        column += 1;
        return create_token(Token::Kind::End, "EOF");
    }

    case '=':
    {
        consume_next_char();

        if (peek_next_char() == '=')
        {
            consume_next_char();
            return create_token(Token::Kind::DoubleEquals, "==");
        }
        else
        {
            return create_token(Token::Kind::Equals, "=");
        }
    }
    case '+':
    {
        consume_next_char();
        return create_token(Token::Kind::Plus, "+");
    }
    case '-':
    {
        consume_next_char();

        if (peek_next_char() == '>')
        {
            consume_next_char();
            return create_token(Token::Kind::Arrow, "->");
        }
        else
        {
            return create_token(Token::Kind::Minus, "-");
        }
    }
    case '*':
    {
        consume_next_char();
        return create_token(Token::Kind::Multiplication, "*");
    }
    case '/':
    {
        consume_next_char();

        return create_token(Token::Kind::Division, "/");
    }
    case '>':
    {
        consume_next_char();

        if (peek_next_char() == '=')
        {
            consume_next_char();
            return create_token(Token::Kind::GreaterThanOrEqual, ">=");
        }
        else
        {
            return create_token(Token::Kind::GreaterThan, ">");
        }
    }
    case '<':
    {
        consume_next_char();

        if (peek_next_char() == '=')
        {
            consume_next_char();
            return create_token(Token::Kind::LesserThanOrEqual, "<=");
        }
        else
        {
            return create_token(Token::Kind::LesserThan, "<");
        }
    }
    case '(':
    {
        consume_next_char();
        return create_token(Token::Kind::LeftParentheses, "(");
    }
    case ')':
    {
        consume_next_char();
        return create_token(Token::Kind::RightParentheses, ")");
    }
    case '{':
    {
        consume_next_char();
        return create_token(Token::Kind::LeftCurlyBrace, "{");
    }
    case '}':
    {
        consume_next_char();
        return create_token(Token::Kind::RightCurlyBrace, "}");
    }
    case '[':
    {
        consume_next_char();
        return create_token(Token::Kind::LeftSquareBracket, "[");
    }
    case ']':
    {
        consume_next_char();
        return create_token(Token::Kind::RightSquareBracket, "]");
    }
    case ':':
    {
        consume_next_char();
        return create_token(Token::Kind::Colon, ":");
    }
    case ';':
    {
        consume_next_char();

        if (peek_next_char() == ';')
        {
            consume_next_char();
            multiline_comment = !multiline_comment;
            return create_token(Token::Kind::DoubleSemicolon, ";;");
        }
        else
        {
            inline_comment = true;
            return create_token(Token::Kind::Semicolon, ";");
        }
    }
    case ',':
    {
        consume_next_char();
        return create_token(Token::Kind::Comma, ",");
    }
    case '#':
    {
        consume_next_char();
        return create_token(Token::Kind::Pound, "#");
    }
    }

    if (is_digit(peek_next_char()))
    {
        std::string text;
        while (is_digit(peek_next_char()))
        {
            text.push_back(consume_next_char());
        }

        if (peek_next_char() == '.')
        {
            text.push_back(consume_next_char());
            while (is_digit(peek_next_char()))
            {
                text.push_back(consume_next_char());
            }

            return create_token(Token::Kind::Real, text, std::stod(text));
        }
        else
        {
            return create_token(Token::Kind::Integer, text, std::stol(text));
        }
    }
    else if (is_identifier_char(peek_next_char()))
    {
        std::string text;
        while (is_identifier_char(peek_next_char()))
        {
            text.push_back(consume_next_char());
        }

        // NOTE: Keywords
        if (text == "function")
        {
            return create_token(Token::Kind::Function, text);
        }
        else if (text == "return")
        {
            return create_token(Token::Kind::Return, text);
        }
        else if (text == "if")
        {
            return create_token(Token::Kind::If, text);
        }
        else if (text == "for")
        {
            return create_token(Token::Kind::For, text);
        }
        else if (text == "true")
        {
            return create_token(Token::Kind::True, text, 1L);
        }
        else if (text == "false")
        {
            return create_token(Token::Kind::False, text, 0L);
        }
        else
        {
            return create_token(Token::Kind::Identifier, text);
        }
    }

    std::string s(1, consume_next_char());
    return create_token(Token::Kind::Unexpected, s);
}

Token Tokenizer::peek() { return peek(1); }

Token Tokenizer::peek(int n) { return tokens[token_index + n]; }

Token Tokenizer::eat() { return tokens[++token_index]; }
