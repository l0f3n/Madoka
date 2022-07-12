#pragma once

#include "AST/AST.h"
#include "CodeGenerator/CodeGenerator.h"
#include "Quads/Quads.h"
#include "SymbolTable/SymbolTable.h"
#include "Tokenizer/Tokenizer.h"
#include "TypeChecker/TypeChecker.h"
#include <iostream>

class Parser
{
  public:
    Parser(Tokenizer &, SymbolTable *, TypeChecker &, Quads &, CodeGenerator &);

    AST_Node *parse();

  private:
    Token expect(Token::Kind kind);

    AST_Node *parse_start();

    AST_StatementList *parse_statement_list();
    AST_Statement     *parse_statement();

    // Function call
    AST_FunctionCall   *parse_function_call();
    AST_ExpressionList *parse_optional_argument_list();
    AST_ExpressionList *parse_argument_list();

    // Function defintion
    AST_ParameterList *parse_optional_parameter_list();
    AST_ParameterList *parse_parameter_list();
    AST_Identifier    *parse_optional_return();
    AST_Identifier    *parse_parameter();

    // Expressions
    AST_Expression *parse_expression();
    AST_Expression *parse_optional_expression();
    AST_Expression *parse_term();
    AST_Expression *parse_minus_term();

    AST_BinaryOperation *respect_precedence(AST_BinaryOperation *binop);

    Tokenizer     tokenizer;
    SymbolTable  *symbol_table;
    TypeChecker   type_checker;
    Quads         quads;
    CodeGenerator code_generator;
};
