#pragma once

#include "Token.h"
#include <iostream>
#include <string>
#include <vector>

class Tokenizer
{
  public:
    Tokenizer(std::istream &is);
    void tokenize();

    Token peek();
    Token peek(int i);
    Token eat();

    void print(std::ostream &os);

  private:
    char peek_next_char();
    char consume_next_char();
    void consume_whitespace();

    bool is_digit(char c);
    bool is_identifier_char(char c);

    Token create_token(Token::Kind kind);
    Token create_token(Token::Kind kind, std::string text);
    Token create_token(Token::Kind kind, std::string text, long value);
    Token create_token(Token::Kind kind, std::string text, double value);

    Token tokenize_next_token();

    std::istream &is;

    int line;
    int column;       // The next char we haven't consumed yet
    int begin_column; // The start column of the current token

    std::vector<Token> tokens{};
    int                token_index{-1};
};
