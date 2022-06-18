#include "AST/AST.h"
#include "CodeGenerator/CodeGenerator.h"
#include "Parser/Parser.h"
#include <fstream>

int main(int argc, char **argv)
{
    std::ifstream is{argv[1], std::ifstream::binary};
    std::cout << std::endl;

    Tokenizer tokenizer{is};
    tokenizer.tokenize();
    tokenizer.print(std::cout);
    std::cout << std::endl;

    SymbolTable symbolTable{};

    Parser    parser{tokenizer, &symbolTable};
    AST_Node *root = parser.parse();
    root->print(std::cout);
    std::cout << std::endl;

    Quads quads{&symbolTable};
    quads.generate_quads(root);
    std::cout << quads << std::endl;

    symbolTable.print(std::cout);
    std::cout << std::endl;

    CodeGenerator code_generator{&symbolTable};
    std::ofstream os{"out.asm"};
    code_generator.generate_code(&quads, os);
}
