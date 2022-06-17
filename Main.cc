#include "AST/AST.h"
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

    Parser    parser{tokenizer};
    AST_Node *expr = parser.parse();
    expr->print(std::cout);

    std::cout << std::endl;

    std::cout << "Value: " << expr->evaluate(1) << std::endl;
}
