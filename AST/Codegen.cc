#include "AST/AST.h"
#include "CodeGenerator.h"
#include <iostream>

void CodeGenerator::generate_code(std::ostream &os) {}

void AST_Node::generate_code(std::ostream &os)
{
    os << "section .text" << std::endl;
    os << "global _start" << std::endl;
    os << std::endl;

    os << "_start:";
}

void AST_Identifier::generate_code(std::ostream &os) {}
void AST_FunctionCall::generate_code(std::ostream &os) {}
void AST_Number::generate_code(std::ostream &os) {}
void AST_UnaryMinus::generate_code(std::ostream &os) {}

void AST_Plus::generate_code(std::ostream &os) { AST_Node::generate_code(os); }

void AST_Minus::generate_code(std::ostream &os) {}
void AST_Multiplication::generate_code(std::ostream &os) {}
void AST_Division::generate_code(std::ostream &os) {}
