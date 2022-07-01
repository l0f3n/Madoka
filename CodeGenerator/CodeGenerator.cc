#include "CodeGenerator.h"
#include "AST/AST.h"
#include "Error/Error.h"
#include "SymbolTable/Symbol.h"
#include "SymbolTable/SymbolTable.h"
#include <fstream>
#include <iostream>
#include <string>

CodeGenerator::CodeGenerator(std::ostream &out, SymbolTable *symbol_table)
    : out{out}, symbol_table{symbol_table}
{
    std::ifstream is{"print.asm"};
    out << is.rdbuf() << std::endl;

    generate_entry_code();
}

void CodeGenerator::generate_predefined_functions() const
{
    // NOTE: Generate a function that can be called that in turn calls the
    // function __print defined in "print.asm"
    FunctionSymbol *print =
        symbol_table->get_function_symbol(symbol_table->lookup_symbol("print"));

    generate_function_prologue(print);

    std::string a1 = address(symbol_table->lookup_symbol("message"));
    operation("mov rax, [" + a1 + "]");

    // TODO: When we call print, we want it to automatically call the correct
    // function for printing an integer, a real or a string. In other words,
    // some sort of function overloading.
    operation("call __print_integer");

    out << std::endl;

    generate_function_epilogue(print);
}

void CodeGenerator::operation(std::string const instruction) const
{
    out << "\t" << instruction << std::endl;
}

void CodeGenerator::label(std::string const name) const
{
    out << std::endl << "L" << name << ":" << std::endl;
}

void CodeGenerator::label(FunctionSymbol const *function) const
{
    out << std::endl
        << "L" << function->label << ":\t; " << function->name << std::endl;
}

std::string CodeGenerator::address(int symbol_index) const
{
    ASSERT(symbol_index != -1);

    Symbol         *symbol = symbol_table->get_symbol(symbol_index);
    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    // NOTE: Right now, we don't support defining functions inside other
    // functions, so we can only access variables in the current scope
    ASSERT(symbol->level - 1 == function->level);

    int offset;

    if (symbol->tag == Symbol::Tag::Variable)
    {
        VariableSymbol *variable =
            symbol_table->get_variable_symbol(symbol_index);

        // NOTE: +8 since we store RBP at the bottom of the activation record
        // which the offset doesn't account for
        offset = -(variable->offset + 8);
    }
    else if (symbol->tag == Symbol::Tag::Parameter)
    {
        ParameterSymbol *parameter =
            symbol_table->get_parameter_symbol(symbol_index);

        // NOTE: The parameters to a function gets pushed onto the stack before
        // it is called, so we can access them at the top of the prevoius
        // activation record. But we need to go down 2: one for previous rbp
        // which is pushed onto the bottom of the stack and one for the return
        // value then, then we are at the top of the stack.
        offset = (function->parameter_count - (parameter->index + 1)) * 8 + 16;
    }
    else
    {
        report_internal_compiler_error(
            "local_variable_offset() called with non parameter/function");
        return "";
    }

    return (offset > 0 ? "rbp+" : "rbp") + std::to_string(offset);
}

void CodeGenerator::load(std::string reg, int symbol_index) const
{
    ASSERT(symbol_index != -1);

    operation("mov " + reg + ", [" + address(symbol_index) + "]");
}

void CodeGenerator::store(int symbol_index, std::string reg) const
{
    ASSERT(symbol_index != -1);

    operation("mov [" + address(symbol_index) + "], " + reg);
}

void CodeGenerator::generate_code(Quads &quads)
{
    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    generate_function_prologue(function);

    Quad *quad = quads.get_current_quad();

    while (quad != nullptr)
    {

#if MA_ASM_COM == 1
        out << "\t;; " << quad->operation << std::endl;
#endif

        switch (quad->operation)
        {
        case Quad::Operation::ASSIGN:
        {
            load("r10", quad->operand1);
            store(quad->dest, "r10");

            break;
        }
        case Quad::Operation::ARGUMENT:
        {
            load("r10", quad->operand1);
            operation("push r10");

            break;
        }
        case Quad::Operation::I_STORE:
        {
            operation("mov qword [" + address(quad->dest) + "], " +
                      std::to_string(quad->operand1));

            break;
        }
        case Quad::Operation::FUNCTION_CALL:
        {
            FunctionSymbol *function =
                symbol_table->get_function_symbol(quad->operand1);

            operation("call L" + std::to_string(function->label));

            if (function->parameter_count > 0)
            {
                // NOTE: Before we call this function we have pushed all the
                // arguments on top of the stack, but after we return we don't
                // need them anymore and can safely decrease the stack pointer
                operation("add rsp, " +
                          std::to_string(function->parameter_count * 8));
            }

            // NOTE: Only store return value on stack if the function actually
            // returns a value.
            if (function->type != symbol_table->type_void)
            {
                ASSERT(quad->dest != -1);

                store(quad->dest, "rax");
            }

            break;
        }
        case Quad::Operation::RETURN:
        {
            if (quad->operand1 != -1)
            {
                load("rax", quad->operand1);
            }

            generate_function_epilogue(function);

            break;
        }
        case Quad::Operation::I_ADD:
        {
            load("r8", quad->operand1);
            load("r9", quad->operand2);
            operation("add r8, r9");
            store(quad->dest, "r8");

            break;
        }
        default:
        {
            report_internal_compiler_error(
                "generate_code(): Unhandled quad type");
        }
        }

#if MA_ASM_COM == 1
        out << std::endl;
#endif

        quad = quads.get_current_quad();
    }

    // NOTE: We need to generate an implicit return if the function body does
    // not contain an explicit return statement
    if (!function->has_return)
    {
        generate_function_epilogue(function);
    }
}

void CodeGenerator::generate_function_prologue(FunctionSymbol *function) const
{
    ASSERT(function != nullptr);

    label(function);

    // ===================================
    // ===== Setup actication record =====
    // ===================================

#if MA_ASM_COM == 1
    out << "\t;; Prologue" << std::endl;
#endif

    operation("push rbp");     // Push previous frames RBP
    operation("mov rbp, rsp"); // Set RBP to current frame

    // NOTE: Allocate space on the activation record for all variables and
    // temporary variables used in the function
    if (function->activation_record_size > 0)
    {
        std::string AR_size = std::to_string(function->activation_record_size);
        operation("sub rsp, " + AR_size);
    }

#if MA_ASM_COM == 1
    out << std::endl;
#endif
}

void CodeGenerator::generate_function_epilogue(FunctionSymbol *function) const
{
    ASSERT(function != nullptr);

#if MA_ASM_COM == 1
    out << "\t;; Epilogue" << std::endl;
#endif

    // TODO: Also check that the function returns no values, otherwise this
    // implementation is not okay.

    // NOTE: Deallocate all the space we allocated in the prologue
    if (function->activation_record_size > 0)
    {
        std::string AR_size = std::to_string(function->activation_record_size);
        operation("add rsp, " + AR_size);
    }

    operation("pop rbp"); // Set RBP to previous frame, since we are returning
    operation("ret");

#if MA_ASM_COM == 1
    out << std::endl;
#endif
}

void CodeGenerator::generate_entry_code() const
{
    out << "section .text" << std::endl;

    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    operation("global _start");

    out << std::endl << "_start:" << std::endl;

    operation("call L" + std::to_string(function->label));

    label("_EXIT");

    operation("mov rax, 0x3c");
    operation("mov rdi, 0x00");
    operation("syscall");
}
