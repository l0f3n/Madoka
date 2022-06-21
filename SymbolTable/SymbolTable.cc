#include "SymbolTable.h"
#include "Error/Error.h"
#include "SymbolTable/Symbol.h"
#include "Tokenizer/Token.h"
#include <string>

SymbolTable::SymbolTable()
    : symbol_table{}, block_table{}, current_symbol_index{-1}
{
    for (int i{0}; i < MAX_HASH_VALUE; ++i)
    {
        hash_table[i] = -1;
    }

    Location location{-1, -1, -1};

    insert_type(location, "void", 0);
    insert_type(location, "integer", 8);
    insert_type(location, "real", 8);
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

Symbol *SymbolTable::generate_temporary_variable()
{
    // TODO: Implement this
    /*
    Symbol *temp = new Symbol(std::to_string(1), "Unknown");
    add_symbol(temp);
    return temp;
    */
    return nullptr;
}

long SymbolTable::get_next_label()
{

    // TODO: Implement this
    // return ++label_number;
    return 0;
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

    // TODO: Assert non-null pointer
    VariableSymbol *variable_symbol = dynamic_cast<VariableSymbol *>(symbol);

    variable_symbol->tag  = Symbol::Tag::Variable;
    variable_symbol->type = type;

    // TODO: Assert non-null pointer
    FunctionSymbol *function_symbol =
        dynamic_cast<FunctionSymbol *>(symbol_table[enclosing_scope()]);
    variable_symbol->offset = function_symbol->activation_record_size;

    // TODO: Assert non-null pointer
    TypeSymbol *type_symbol = dynamic_cast<TypeSymbol *>(symbol_table[type]);
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

    // NOTE: At this point, this is guaranteed to be a FunctionSymbol
    // TODO: Assert non-null pointer
    FunctionSymbol *function_symbol = dynamic_cast<FunctionSymbol *>(symbol);

    function_symbol->tag   = Symbol::Tag::Function;
    function_symbol->label = get_next_label();
    // TODO: Set type, somehow, as well

    return symbol_index;
}

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

    // NOTE: At this point, this is guaranteed to be a TypeSymbol
    // TODO: Assert non-null pointer
    TypeSymbol *type_symbol = dynamic_cast<TypeSymbol *>(symbol);

    type_symbol->tag  = Symbol::Tag::Type;
    type_symbol->size = size;
    type_symbol->type = 0; // Void type, the first one we add

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

Symbol *SymbolTable::get_symbol(int symbol_index) const
{
    return symbol_table[symbol_index];
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
