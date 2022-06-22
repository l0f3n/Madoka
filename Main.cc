#include "AST/AST.h"
#include "CodeGenerator/CodeGenerator.h"
#include "Parser/Parser.h"
#include "TypeChecker/TypeChecker.h"
#include <fstream>

int main(int argc, char **argv)
{

    std::ifstream is{argv[1], std::ifstream::binary};
    std::cout << std::endl;

    Tokenizer tokenizer{is};
    tokenizer.tokenize();
    tokenizer.print(std::cout);
    std::cout << std::endl;

    SymbolTable symbol_table{};
    TypeChecker type_checker{&symbol_table};
    Quads       quads{&symbol_table};

    Parser    parser{tokenizer, &symbol_table, type_checker, quads};
    AST_Node *root = parser.parse();

    symbol_table.print(std::cout);
    std::cout << std::endl;

    /*
    Quads quads{&symbol_table};
    quads.generate_quads(root);
    std::cout << quads << std::endl;
    */

    /*
    CodeGenerator code_generator{&symbol_table};
    std::ofstream os{"out.asm"};
    code_generator.generate_code(&quads, os);
    */
}
