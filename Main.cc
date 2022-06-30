#include "AST/AST.h"
#include "CodeGenerator/CodeGenerator.h"
#include "Parser/Parser.h"
#include "TypeChecker/TypeChecker.h"
#include <chrono>
#include <fstream>
#include <iomanip>
#include <string>

using namespace std::chrono;

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        std::cout << "Please provide an input file" << std::endl;
        std::exit(0);
    }

    bool quiet = false;
    for (int i = 2; i < argc; i++)
    {
        if (argv[i] == std::string("--quiet"))
        {
            quiet = true;
        }
    }

    auto t1 = high_resolution_clock::now();

    std::ifstream is{argv[1], std::ifstream::binary};

    Tokenizer tokenizer{is};

    tokenizer.tokenize();

    SymbolTable symbol_table{};
    TypeChecker type_checker{&symbol_table};
    Quads       quads{&symbol_table};

    std::ofstream os{"out.asm"};
    CodeGenerator code_generator{os, &symbol_table};

    Parser parser{tokenizer, &symbol_table, type_checker, quads,
                  code_generator};

    parser.parse();

    auto t2 = high_resolution_clock::now();

    int status_code = std::system("nasm -f elf64 -o out.o out.asm");

    if (status_code != 0)
    {
        std::exit(status_code);
    }

    auto t3 = high_resolution_clock::now();

    status_code = std::system("ld -o out out.o");

    if (status_code != 0)
    {
        std::exit(status_code);
    }

    auto t4 = high_resolution_clock::now();

    if (!quiet)
    {
        duration<float> d1 = t4 - t1;
        std::cout << "Compiled in: " << std::setprecision(4) << std::fixed
                  << d1.count() << " seconds" << std::endl;

        duration<float> d2 = t2 - t1;
        std::cout << "\tGenerated assembler in: " << std::setprecision(4)
                  << std::fixed << d2.count() << " seconds" << std::endl;

        duration<float> d3 = t3 - t2;
        std::cout << "\tAssembled assembler in: " << std::setprecision(4)
                  << std::fixed << d3.count() << " seconds" << std::endl;

        duration<float> d4 = t4 - t3;
        std::cout << "\tLinked object file in:  " << std::setprecision(4)
                  << std::fixed << d4.count() << " seconds" << std::endl;
    }
}
