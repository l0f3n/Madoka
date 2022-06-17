#include "Token.h"
#include <string>

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
    case Token::Kind::Number: return os << "Number";
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
