#include "SymbolTable.h"
#include "Error/Error.h"
#include "SymbolTable/Symbol.h"
#include "Tokenizer/Token.h"
#include <sstream>
#include <string>

SymbolTable::SymbolTable()
{
    std::fill(std::begin(symbol_table), std::end(symbol_table), nullptr);
    std::fill(std::begin(block_table), std::end(block_table), -1);
    std::fill(std::begin(hash_table), std::end(hash_table), -1);

    type_void    = insert_type(no_location, "void", 0);
    type_integer = insert_type(no_location, "int", 8);
    type_real    = insert_type(no_location, "real", 8);

    // NOTE: Insert a default global function that always gets called in the
    // beginning of the program. This is a bit slower but makes everything else
    // a lot easier since you can always assume there is an enclosing scope.
    // This is the "global scope".
    insert_function(no_location, "#global");
    open_scope();
}

SymbolTable::~SymbolTable()
{
    for (int i{0}; i < MAX_SYMBOLS; ++i)
    {
        if (symbol_table[i] != nullptr)
        {
            delete symbol_table[i];
        }
    }
}

void SymbolTable::print(std::ostream &os)
{
    for (int i{0}; i <= current_symbol_index; i++)
    {
        os << i << ": " << *symbol_table[i] << std::endl;
    }
}

int SymbolTable::generate_temporary_variable(int type)
{
    if (type == type_void)
    {
        internal_compiler_error()
            << "Cannot generate a temporary variable of type void" << std::endl;
        std::exit(1);
    }

    std::ostringstream oss{};
    oss << "#TEMP_" << ++current_temporary_variable_number;

    return insert_variable(no_location, oss.str(), type);
}

int SymbolTable::get_next_label() { return ++current_label_number; }

int SymbolTable::insert_type(Location const &location, const std::string &name,
                             int size)
{
    int symbol_index = insert_symbol(location, name, Symbol::Tag::Type);

    Symbol *symbol = symbol_table[symbol_index];

    if (symbol->tag != Symbol::Tag::Undefined)
    {
        error(location) << "Type '" << name << "' already defined at "
                        << symbol->location << std::endl;
        std::exit(1);
    }

    TypeSymbol *type_symbol = get_type_symbol(symbol_index);

    type_symbol->tag  = Symbol::Tag::Type;
    type_symbol->size = size;
    type_symbol->type = 0; // Void type, the first one we add

    return symbol_index;
}

int SymbolTable::insert_variable(Location const    &location,
                                 const std::string &name, int type)
{
    int symbol_index = insert_symbol(location, name, Symbol::Tag::Variable);

    Symbol *symbol = symbol_table[symbol_index];

    if (symbol->tag != Symbol::Tag::Undefined)
    {
        error(location) << "Variable '" << name << "' already defined at "
                        << symbol->location << std::endl;

        std::exit(1);
        return symbol_index;
    }

    VariableSymbol *variable_symbol = get_variable_symbol(symbol_index);

    variable_symbol->tag  = Symbol::Tag::Variable;
    variable_symbol->type = type;

    FunctionSymbol *function_symbol = get_function_symbol(enclosing_scope());
    TypeSymbol     *type_symbol     = get_type_symbol(type);

    variable_symbol->offset = function_symbol->activation_record_size;
    function_symbol->activation_record_size += type_symbol->size;

    return symbol_index;
}

int SymbolTable::insert_function(Location const    &location,
                                 const std::string &name)
{
    int symbol_index = insert_symbol(location, name, Symbol::Tag::Function);

    Symbol *symbol = symbol_table[symbol_index];

    if (symbol->tag != Symbol::Tag::Undefined)
    {
        error(location) << "Function '" << name << "' already defined at "
                        << symbol->location << std::endl;
        std::exit(1);
    }

    FunctionSymbol *function_symbol = get_function_symbol(symbol_index);

    function_symbol->tag   = Symbol::Tag::Function;
    function_symbol->label = get_next_label();

    // NOTE: We already set this in the constructor, but its good to be
    // explicit. The function doesn't know which one is its last parameter when
    // we add it, when we actually add the parameters later they will set it
    function_symbol->first_parameter = -1;

    return symbol_index;
}

int SymbolTable::insert_parameter(Location const    &location,
                                  const std::string &name, int type)
{
    int symbol_index = insert_symbol(location, name, Symbol::Tag::Parameter);

    Symbol *symbol = symbol_table[symbol_index];

    if (symbol->tag != Symbol::Tag::Undefined)
    {
        error(location) << "Parameter'" << name << "' already defined at "
                        << symbol->location << std::endl;
        std::exit(1);
    }

    ParameterSymbol *parameter_symbol = get_parameter_symbol(symbol_index);

    parameter_symbol->tag  = Symbol::Tag::Parameter;
    parameter_symbol->type = type;

    FunctionSymbol *function_symbol = get_function_symbol(enclosing_scope());

    int parameter_count = 0;

    // NOTE: This is probably pretty slow. We go through every parameter so that
    // we can set this as the next parameter to the previous last parameter
    if (function_symbol->first_parameter == -1)
    {
        function_symbol->first_parameter = symbol_index;
    }
    else
    {
        ParameterSymbol *parameter =
            get_parameter_symbol(function_symbol->first_parameter);

        while (parameter->next_parameter != -1)
        {
            parameter_count += 1;
            parameter = get_parameter_symbol(parameter->next_parameter);
        }

        parameter_count += 1;
        parameter->next_parameter = symbol_index;
    }

    parameter_symbol->index = parameter_count;

    TypeSymbol *type_symbol = get_type_symbol(type);

    parameter_symbol->offset = function_symbol->activation_record_size;
    function_symbol->activation_record_size += type_symbol->size;

    return symbol_index;
}

int SymbolTable::insert_symbol(Location const    &location,
                               std::string const &name, Symbol::Tag tag)
{
    int symbol_index = lookup_symbol(name);

    if (symbol_index != -1 &&
        ((symbol_table[symbol_index]->level == current_level) ||
         symbol_table[symbol_index]->tag == Symbol::Tag::Type))
    {
        // NOTE: A symbol with the same name already exists on the same
        // level, so instead of creating a new one we return the already
        // defined one and let the caller handle it. Also, you can never use
        // the same name as a type, regardless of its level.
        return symbol_index;
    }

    Symbol *symbol;

    switch (tag)
    {
    case Symbol::Tag::Variable:
    {
        symbol = new VariableSymbol(location, name);
        break;
    }
    case Symbol::Tag::Function:
    {
        symbol = new FunctionSymbol(location, name);
        break;
    }
    case Symbol::Tag::Type:
    {
        symbol = new TypeSymbol(location, name);
        break;
    }
    case Symbol::Tag::Parameter:
    {
        symbol = new ParameterSymbol(location, name);
        break;
    }
    default:
    {
        internal_compiler_error() << " Unhandled symbol tag '" << tag
                                  << "' in insert_symbol()" << std::endl;
        std::exit(1);
    }
    }

    symbol->level = current_level;

    if (++current_symbol_index >= MAX_SYMBOLS)
    {
        std::cout << "Erorr: Maximum amount of symbols exceeded" << std::endl;
    }

    symbol_table[current_symbol_index] = symbol;
    hash_table[hash(name)]             = current_symbol_index;

    if (symbol_index != -1)
    {
        symbol->hash_link = symbol_index;
    }

    return current_symbol_index;
}

/*
int SymbolTable::insert_symbol(std::string const &name, Symbol::Tag tag)
{
// NOTE: Dummy location, used for temporary variables and predefined types
Location const location{-1, -1, -1};
return insert_symbol(location, name, tag);
}
*/

int SymbolTable::lookup_symbol(const std::string &name) const
{
    int found_index = hash_table[hash(name)];

    while (found_index != -1)
    {
        Symbol *symbol = symbol_table[found_index];

        if (symbol->name == name)
        {
            return found_index;
        }
        else
        {
            found_index = symbol->hash_link;
        }
    }

    return -1;
}

TypeSymbol *SymbolTable::get_type_symbol(int symbol_index) const
{
    Symbol     *symbol      = get_symbol(symbol_index);
    TypeSymbol *type_symbol = dynamic_cast<TypeSymbol *>(symbol);
    ASSERT(type_symbol != nullptr);
    return type_symbol;
}

VariableSymbol *SymbolTable::get_variable_symbol(int symbol_index) const
{
    Symbol         *symbol          = get_symbol(symbol_index);
    VariableSymbol *variable_symbol = dynamic_cast<VariableSymbol *>(symbol);
    ASSERT(variable_symbol != nullptr);
    return variable_symbol;
}

FunctionSymbol *SymbolTable::get_function_symbol(int symbol_index) const
{
    Symbol         *symbol          = get_symbol(symbol_index);
    FunctionSymbol *function_symbol = dynamic_cast<FunctionSymbol *>(symbol);
    ASSERT(function_symbol != nullptr);
    return function_symbol;
}

ParameterSymbol *SymbolTable::get_parameter_symbol(int symbol_index) const
{
    Symbol          *symbol           = get_symbol(symbol_index);
    ParameterSymbol *parameter_symbol = dynamic_cast<ParameterSymbol *>(symbol);
    ASSERT(parameter_symbol != nullptr);
    return parameter_symbol;
}

Symbol *SymbolTable::get_symbol(int symbol_index) const
{
    Symbol *symbol = symbol_table[symbol_index];
    ASSERT(symbol != nullptr);
    return symbol;
}

Symbol *SymbolTable::remove_symbol(int symbol_index)
{
    Symbol *symbol     = symbol_table[symbol_index];
    int     hash_value = hash(symbol->name);

    if (hash_table[hash_value] == symbol_index)
    {
        hash_table[hash_value] = symbol->hash_link;
        symbol->hash_link      = -1;
    }

    return symbol;
}

int SymbolTable::hash(const std::string &name) const
{
    // Source:
    // https://www.cs.hmc.edu/~geoff/classes/hmc.cs070.200101/homework10/hashfuncs.html

    int h{0};

    for (char c : name)
    {
        int highorder = h & 0xf8000000;
        h             = h << 5;
        h             = h ^ (highorder >> 27);
        h             = h ^ c;
    }

    return h % MAX_HASH_VALUE;
}

void SymbolTable::open_scope()
{
    if (++current_level >= MAX_LEVELS)
    {
        std::cout << "Error: Maximum amount of levels exceeded" << std::endl;
    }

    block_table[current_level] = current_symbol_index;
}

void SymbolTable::close_scope()
{
    for (int i{current_symbol_index}; i > block_table[current_level]; i--)
    {
        remove_symbol(i);
    }

    current_level -= 1;
}

int SymbolTable::enclosing_scope() const { return block_table[current_level]; }

std::string SymbolTable::get_name(int symbol_index)
{
    ASSERT(symbol_index >= 0);
    Symbol *symbol = symbol_table[symbol_index];
    ASSERT(symbol != nullptr);
    return symbol->name;
}
