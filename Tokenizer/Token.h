#pragma once

#include <iostream>
#include <string>

struct Location
{
    int l1; // Starting line
    int c1; // Starting column
    int c2; // Ending column
};

struct Token
{
  public:
    enum class Kind
    {
        Identifier,
        Integer,
        Real,

        // Math
        Plus,
        Minus,
        Multiplication,
        Division,

        // Comparison
        GreaterThan,
        LesserThan,

        // Keywords
        Function,
        If,
        Return,
        While,

        // Characters
        LeftParentheses,
        RightParentheses,
        LeftCurlyBrace,
        RightCurlyBrace,
        Equals,
        Comma,
        Colon,
        Arrow,

        // Special
        Unitialized,
        Unexpected,
        End,
    };

    Token();
    Token(Kind, int, int, int, std::string, long);
    Token(Kind, int, int, int, std::string, double);

    Kind     kind;
    Location location;

    /*
    int line;
    int begin_column;
    int end_column;
    */

    std::string text{""};
    union
    {
        long   integer_value;
        double real_value;
    };

    friend std::ostream &operator<<(std::ostream &os, Token const &t);
};

std::ostream &operator<<(std::ostream &os, Token::Kind const &k);
std::ostream &operator<<(std::ostream &os, Location const &l);
