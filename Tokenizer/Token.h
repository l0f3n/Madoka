#pragma once

#include <iostream>
#include <string>

struct Token
{
  public:
    enum class Kind
    {
        Identifier,
        Integer,
        Real,

        Plus,
        Minus,
        Multiplication,
        Division,
        Exponentiation,

        Equals,

        LeftParentheses,
        RightParentheses,

        Unitialized,
        Unexpected,
        End,
    };

    Token();
    Token(Kind, int, int, int, std::string, long);
    Token(Kind, int, int, int, std::string, double);

    Kind kind;

    int line;
    int begin_column;
    int end_column;

    std::string text{""};
    union
    {
        long   integer_value;
        double real_value;
    };

    friend std::ostream &operator<<(std::ostream &os, Token const &t);
};

std::ostream &operator<<(std::ostream &os, Token::Kind const &k);
