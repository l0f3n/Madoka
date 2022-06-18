#include "Token.h"
#include <string>

Token::Token()
    : kind{Token::Kind::Unitialized}, line{0}, begin_column(0),
      end_column(0), text{""}, integer_value{0}
{}

Token::Token(Kind kind, int line, int begin_column, int end_column,
             std::string text, long integer_value)
    : kind{kind}, line{line}, begin_column{begin_column},
      end_column{end_column}, text{text}, integer_value{integer_value}
{}

Token::Token(Kind kind, int line, int begin_column, int end_column,
             std::string text, double real_value)
    : kind{kind}, line{line}, begin_column{begin_column},
      end_column{end_column}, text{text}, real_value{real_value}
{}

std::ostream &operator<<(std::ostream &os, Token const &t)
{
    os << "Token<" << t.line + 1 << ", " << t.begin_column + 1 << "-"
       << t.end_column + 1 << ", " << t.kind;

    if (t.text != "")
    {
        os << ", '" << t.text << "'";
    }

    os << ">";

    return os;
}

std::ostream &operator<<(std::ostream &os, Token::Kind const &k)
{
    switch (k)
    {
    case Token::Kind::Identifier: return os << "Identifier";
    case Token::Kind::Integer: return os << "Integer";
    case Token::Kind::Real: return os << "Real";
    case Token::Kind::Plus: return os << "Plus";
    case Token::Kind::Minus: return os << "Minus";
    case Token::Kind::Multiplication: return os << "Multiplication";
    case Token::Kind::Division: return os << "Division";
    case Token::Kind::Exponentiation: return os << "Exponentiation";
    case Token::Kind::Equals: return os << "Equals";
    case Token::Kind::LeftParentheses: return os << "LeftParentheses";
    case Token::Kind::RightParentheses: return os << "RightParentheses";
    case Token::Kind::End: return os << "End";
    default: return os << "Unexpected";
    }
}
