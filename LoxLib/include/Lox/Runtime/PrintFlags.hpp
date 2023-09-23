#ifndef LOX_VM_RUNTIME_PRINT_FLAGS_HPP
#define LOX_VM_RUNTIME_PRINT_FLAGS_HPP

namespace Lox
{
    // Looked up from CPython.
    enum class PrintFlags
    {
        Raw,   // Strings will be printed with double quotes.
        Pretty // String will be printed without quotes.
    };
}

#endif // LOX_VM_RUNTIME_PRINT_FLAGS_HPP
