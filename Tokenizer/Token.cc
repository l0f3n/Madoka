#include "Token.h"
#include <string>

Token::Token()
    : kind{Token::Kind::Unitialized}, location{Location{0, 0, 0}}, text{""},
      integer_value{0}
{}

Token::Token(Kind kind, int line, int begin_column, int end_column,
             std::string text, long integer_value)
    : kind{kind}, location{Location{line, begin_column, end_column}},
      text{text}, integer_value{integer_value}
{}

Token::Token(Kind kind, int line, int begin_column, int end_column,
             std::string text, double real_value)
    : kind{kind}, location{Location{line, begin_column, end_column}},
      text{text}, real_value{real_value}
{}

std::ostream &operator<<(std::ostream &os, Token const &t)
{
    Location loc = t.location;
    os << "Token<" << loc.l1 << ", ";

    if (loc.c1 != loc.c2)
    {
        os << loc.c1 << "-" << loc.c2;
    }
    else
    {
        os << loc.c1;
    }

    os << ", " << t.kind;

    if (t.text != "")
    {
        os << ", '" << t.text << "'";
    }

    os << ">";

    return os;
}

std::ostream &operator<<(std::ostream &os, Location const &l)
{
    return os << l.l1 << "," << l.c1;
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
        case Token::Kind::GreaterThan: return os << "GreaterThan";
        case Token::Kind::LesserThan: return os << "LesserThan";
        case Token::Kind::Equals: return os << "Equals";
        case Token::Kind::Function: return os << "Function";
        case Token::Kind::Return: return os << "Return";
        case Token::Kind::If: return os << "If";
        case Token::Kind::While: return os << "While";
        case Token::Kind::LeftParentheses: return os << "LeftParentheses";
        case Token::Kind::RightParentheses: return os << "RightParentheses";
        case Token::Kind::LeftCurlyBrace: return os << "LeftCurlyBrace";
        case Token::Kind::RightCurlyBrace: return os << "RightCurlyBrace";
        case Token::Kind::Colon: return os << "Colon";
        case Token::Kind::Comma: return os << "Comma";
        case Token::Kind::Arrow: return os << "Arrow";
        case Token::Kind::End: return os << "End";
        default: return os << "Unexpected";
    }
}
