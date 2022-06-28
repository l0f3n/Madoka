#include "CodeGenerator.h"
#include "AST/AST.h"
#include "Error/Error.h"
#include "SymbolTable/Symbol.h"
#include "SymbolTable/SymbolTable.h"
#include <iostream>
#include <string>

CodeGenerator::CodeGenerator(std::ostream &out, SymbolTable *symbol_table)
    : out{out}, symbol_table{symbol_table}
{
    generate_entry_code();
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

int CodeGenerator::local_variable_offset(int symbol_index) const
{
    ASSERT(symbol_index != -1);

    Symbol         *symbol = symbol_table->get_symbol(symbol_index);
    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    // NOTE: Right now, we don't support defining functions inside other
    // functions, so we can only access variables in the current scope
    ASSERT(symbol->level - 1 == function->level);

    if (symbol->tag == Symbol::Tag::Variable)
    {
        return symbol->offset + 8;
    }
    else if (symbol->tag == Symbol::Tag::Parameter)
    {
        return symbol->offset + 8;
    }
    else
    {
        report_internal_compiler_error(
            "local_variable_offset() called with non parameter/function");
        return -1;
    }
}

void CodeGenerator::load(std::string reg, int symbol_index) const
{
    ASSERT(symbol_index != -1);

    int offset = local_variable_offset(symbol_index);
    operation("mov " + reg + ", [rbp-" + std::to_string(offset) + "]");
}

void CodeGenerator::store(int symbol_index, std::string reg) const
{
    ASSERT(symbol_index != -1);

    int offset = local_variable_offset(symbol_index);
    operation("mov [rbp-" + std::to_string(offset) + "], " + reg);
}

std::string CodeGenerator::get_argument_register(int i) const
{
    switch (i)
    {
    case 0:
    {
        return "rdi";
        break;
    }
    case 1:
    {
        return "rsi";
        break;
    }
    case 2:
    {
        return "rdx";
        break;
    }
    case 3:
    {
        return "rcx";
        break;
    }
    case 4:
    {
        return "r8";
        break;
    }
    case 5:
    {
        return "r9";
        break;
    }
    default:
    {
        report_internal_compiler_error("Only 6 arguments are supported");
        return "";
        break;
    }
    }
}

void CodeGenerator::store_parameter(int symbol_index) const
{
    if (symbol_index == -1)
    {
        return;
    }

    ParameterSymbol *parameter =
        symbol_table->get_parameter_symbol(symbol_index);

    store(symbol_index, get_argument_register(parameter->index));

    store_parameter(parameter->next_parameter);
}

void CodeGenerator::generate_code(Quads &quads)
{
    FunctionSymbol *function =
        symbol_table->get_function_symbol(symbol_table->enclosing_scope());

    generate_function_prologue(function);

    Quad *quad = quads.get_current_quad();

    // std::cout << quads << std::endl;

    while (quad != nullptr)
    {
        switch (quad->operation)
        {
            {
            case Quad::Operation::ASSIGN:
#if MA_ASM_COM == 1
                out << "; -- ASSIGN" << std::endl;
#endif

                // TODO: When we do a function call with multiple return values
                // we can't simply do it like this since this only supports one
                // value. A solution would be to make this more general and
                // support multiple regular assignments as well in a single
                // line. This would be fine, but maybe only support one for now.

                // TODO: The current error results from the fact that function
                // calls don't return an index to a value. It can't since it
                // wants to treturn multiple. We need to handle that, see above.
                load("r10", quad->operand1);
                store(quad->dest, "r10");

#if MA_ASM_COM == 1
                out << "; ASSIGN" << std::endl;
#endif

                break;
            }
        case Quad::Operation::ARGUMENT:
        {
#if MA_ASM_COM == 1
            out << "; -- I_ARUGMENT" << std::endl;
#endif
            std::string reg = get_argument_register(quad->operand2);
            load(reg, quad->operand1);
            // operation("push r10");
#if MA_ASM_COM == 1
            out << "; -- I_ARGUMENT" << std::endl;
#endif

            break;
        }
        case Quad::Operation::I_STORE:
        {

#if MA_ASM_COM == 1
            out << "; -- I_STORE" << std::endl;
#endif
            std::string offset =
                std::to_string(local_variable_offset(quad->dest));

            operation("mov qword [rsp-" + offset + "], " +
                      std::to_string(quad->operand1));

            // NOTE: Move integer literal operand1 into dest

            // TODO: We need to use the dest symbol to calcualte where in the
            // AR to put the integer literal

            // out << "; Assignment code" << std::endl;

            // operation("mov " + std::to_string(quad->dest) + ", " +
            // std::to_string(quad->operand1));

#if MA_ASM_COM == 1
            out << "; I_STORE" << std::endl;
#endif

            break;
        }
        case Quad::Operation::FUNCTION_CALL:
        {
#if MA_ASM_COM == 1
            out << "; -- " << quad->operation << std::endl;
#endif

            FunctionSymbol *function =
                symbol_table->get_function_symbol(quad->operand1);

            operation("call L" + std::to_string(function->label));

            // TODO: Only transfer return value from rax to stack if the
            // function we are calling is actually returning a value. It should
            // probably be stored in the function symbol in some way. I believe
            // this is what causes the current segmentation fault.
            store(quad->dest, "rax");

#if MA_ASM_COM == 1
            out << "; FUNCTION_CALL" << std::endl;
#endif

            break;
        }
        case Quad::Operation::RETURN:
        {
#if MA_ASM_COM == 1
            out << "; -- I_RETURN" << std::endl;
#endif
            if (quad->operand1 != -1)
            {
                load("rax", quad->operand1);
            }

            generate_function_epilogue(function);
#if MA_ASM_COM == 1
            out << "; I_RETURN" << std::endl;
#endif

            break;
        }
        case Quad::Operation::I_ADD:
        {
#if MA_ASM_COM == 1
            out << "; -- I_ADD" << std::endl;
#endif

            load("r8", quad->operand1);
            load("r9", quad->operand2);
            operation("add r8, r9");
            store(quad->dest, "r8");

#if MA_ASM_COM == 1
            out << "; I_ADD" << std::endl;
#endif
            break;
        }
        default:
        {
            std::cout << "Unhandled quad type " << *quad << std::endl;
            std::exit(1);
        }
        }

        out << std::endl;

        quad = quads.get_current_quad();
    }

    generate_function_epilogue(function);
}

void CodeGenerator::generate_function_prologue(FunctionSymbol *function) const
{
#if MA_ASM_COM == 1
    out << "; -- PROLOGUE" << std::endl;
#endif

    ASSERT(function != nullptr);

    label(function);

    // ===================================
    // ===== Setup actication record =====
    // ===================================

    operation("push rbp");     // Push previous frames RBP
    operation("mov rbp, rsp"); // Set RBP to current frame

    // NOTE: Allocate space on the activation record for all variables and
    // temporary variables used in the function
    std::string ar_size = std::to_string(function->activation_record_size);
    operation("sub rsp, " + ar_size); // Allocate space for variables

    // NOTE: Puts all received arguments from the registers to their proper
    // location on the stack. Do nothing if function takes 0 parameters.
    store_parameter(function->first_parameter);

    out << std::endl;

#if MA_ASM_COM == 1
    out << "; PROLOGUE" << std::endl;
#endif
}

void CodeGenerator::generate_function_epilogue(FunctionSymbol *function) const
{
#if MA_ASM_COM == 1
    out << "; -- EPILOGUE" << std::endl;
#endif

    ASSERT(function != nullptr);

    // TODO: We don't need to do this if the function contains a return
    // statement, check for that and don't generate anything if that is the
    // case. Also check that the function returns no values, otherwise this
    // implementation is not okay.

    // NOTE: Deallocate all the space we allocated in the prologue
    std::string AR_size = std::to_string(function->activation_record_size);
    operation("add rsp, " + AR_size);
    operation("pop rbp"); // Set RBP to previous frame, since we are returning
    operation("ret");

    out << std::endl;

#if MA_ASM_COM == 1
    out << "; EPILOGUE" << std::endl;
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

/*
void CodeGenerator::generate_used_intrinsics() const
{
    // TODO: This seems like a bad solution, do something else
    for (std::string intrinsic : intrinsics)
    {
        if (intrinsic == "print")
        {
            FunctionSymbol *function = symbol_table->get_function_symbol(
                symbol_table->lookup_symbol(intrinsic));

            generate_function_prologue(function);

            operation("mov rax, 1"); // Write system call
            operation("mov rdi, 1"); // Write to standard out

            load("rsi", function->first_parameter); // Write value

            operation("mov rdx, 8"); // Write length bytes
            operation("syscall");

            generate_function_epilogue(function);
        }
    }
}
*/
