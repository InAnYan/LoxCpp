#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_STACK_OVERFLOW_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_STACK_OVERFLOW_HPP

#include "RuntimeException.hpp"

namespace Lox::Exceptions
{
    class StackOverflow final : public RuntimeException
    {
    public:
        StackOverflow(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace);
    }; // class StackOverflow
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_STACK_OVERFLOW_HPP
