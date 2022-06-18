#include "CodeGenerator.h"
#include "AST/AST.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>

CodeGenerator::CodeGenerator(SymbolTable *symbolTable)
    : symbolTable{symbolTable}
{}

void CodeGenerator::load(std::ostream &os, std::string reg,
                         Symbol *symbol) const
{
    // TODO: This needs to be waay more sophisticaed later when we handle actual
    // functions calls and this the right way.
    os << "\t mov " << reg << ", "
       << "[rsp+" << symbol->offset << "]" << std::endl;
}

void CodeGenerator::store(std::ostream &os, Symbol *symbol,
                          std::string reg) const
{
    // TODO: This needs to be waay more sophisticaed later when we handle actual
    // functions calls and this the right way.
    os << "\t mov "
       << "[rsp+" << symbol->offset << "]"
       << ", " << reg << std::endl;
}

void CodeGenerator::generate_code(Quads *quads, std::ostream &os) const
{
    generate_entry_code(os);

    for (Quad *quad : *quads)
    {
        switch (quad->operation)
        {
        case Quad::Operation::I_ADD:
        {
            load(os, "r8", quad->operand1);
            load(os, "r9", quad->operand2);

            // Add the two values in registers r8 and r9
            os << "\t add r8, r9" << std::endl;
            store(os, quad->dest, "r8");

            break;
        }
        case Quad::Operation::I_LOAD:
        {
            os << "\t mov r8, " << quad->integer_value1 << std::endl;
            store(os, quad->dest, "r8");

            break;
        }
        }
    }

    generate_exit_code(os);
}

void CodeGenerator::generate_entry_code(std::ostream &os) const
{
    os << "section .text" << std::endl;
    os << "\t global _start" << std::endl;
    os << std::endl;
    os << "_start:" << std::endl;
    os << "\t sub rsp, " << symbolTable->offset << std::endl;
}

void CodeGenerator::generate_exit_code(std::ostream &os) const
{
    os << std::endl;
    os << '\t' << "jmp _exit" << std::endl;
    os << std::endl;
    os << "_exit:" << std::endl;
    os << '\t' << "mov rax, 0x3c" << std::endl;
    os << '\t' << "mov rdi, 0x00" << std::endl;
    os << '\t' << "syscall" << std::endl;
}
