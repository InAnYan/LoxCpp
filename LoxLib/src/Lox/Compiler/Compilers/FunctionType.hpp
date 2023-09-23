#ifndef LOX_VM_COMPILER_COMPILERS_FUNCTION_TYPE_HPP
#define LOX_VM_COMPILER_COMPILERS_FUNCTION_TYPE_HPP

namespace Lox
{
    enum class FunctionType
    {
        Script,
        Default,
        Method,
        Initializer,
    };
}

#endif // LOX_VM_COMPILER_COMPILERS_FUNCTION_TYPE_HPP
