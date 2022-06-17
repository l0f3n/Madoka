#pragma once

#include <iostream>
#include <string>

struct Token
{
  public:
    enum class Kind
    {
        Identifier,
        Number,

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

    Kind kind;

    int line;
    int begin_column;
    int end_column;

    std::string text{""};
    double value{};

    friend std::ostream &operator<<(std::ostream &os, Token const &t);
};

std::ostream &operator<<(std::ostream &os, Token::Kind const &k);
