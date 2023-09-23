#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_INVALID_STACK_ACCESS_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_INVALID_STACK_ACCESS_HPP

#include "RuntimeException.hpp"

namespace Lox::Exceptions
{
    class InvalidStackAccess : public RuntimeException
    {
    public:
        InvalidStackAccess(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace);
    };
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_INVALID_STACK_ACCESS_HPP
