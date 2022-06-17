#pragma once

#include "AST/AST.h"
#include "Tokenizer/Tokenizer.h"
#include <iostream>

class Parser
{
  public:
    Parser(std::istream &is);
    Parser(Tokenizer &tokenizer);

    AST_Node *parse();

  private:
    Token expect(Token::Kind kind);
    void  report_syntax_error_and_stop(std::string message);

    AST_Expression      *parse_start();
    AST_Expression      *parse_expression();
    AST_BinaryOperation *parse_expression_tail();
    AST_Expression      *parse_term();
    AST_Expression      *parse_minus_term();
    AST_Expression      *parse_term_tail();

    AST_BinaryOperation *respect_precedence(AST_BinaryOperation *binop);

    Tokenizer tokenizer;
};
