#pragma once

#include "AST/AST.h"
#include "SymbolTable/SymbolTable.h"
#include "Tokenizer/Tokenizer.h"
#include <iostream>

class Parser
{
  public:
    Parser(std::istream &is);
    Parser(Tokenizer &tokenizer, SymbolTable *symbolTable);

    AST_Node *parse();

  private:
    Token expect(Token::Kind kind);
    void  report_syntax_error_and_stop(std::string message);

    AST_Node *parse_start();

    AST_StatementList *parse_statement_list();
    AST_StatementList *parse_statement_list_tail();
    AST_Statement     *parse_statement();
    AST_Statement     *parse_statement_tail();

    // Function call
    AST_ExpressionList *parse_optional_argument_list();
    AST_ExpressionList *parse_argument_list();
    AST_ExpressionList *parse_argument_list_tail();
    AST_Expression     *parse_argument();

    // Function defintion
    AST_ParameterList *parse_optional_parameter_list();
    AST_ParameterList *parse_parameter_list();
    AST_ParameterList *parse_parameter_list_tail();
    AST_ParameterList *parse_optional_return();
    AST_Identifier    *parse_parameter();

    // Expressions
    AST_Expression      *parse_expression();
    AST_BinaryOperation *parse_expression_tail();
    AST_Expression      *parse_term();
    AST_Expression      *parse_minus_term();
    AST_ExpressionList  *parse_term_tail();

    AST_BinaryOperation *respect_precedence(AST_BinaryOperation *binop);

    Tokenizer    tokenizer;
    SymbolTable *symbolTable;
};
