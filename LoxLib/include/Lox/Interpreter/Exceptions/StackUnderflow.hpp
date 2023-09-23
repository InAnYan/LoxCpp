#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_STACK_UNDERFLOW_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_STACK_UNDERFLOW_HPP

#include "RuntimeException.hpp"

namespace Lox::Exceptions
{
    class StackUnderflow final : public RuntimeException
    {
    public:
        StackUnderflow(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace);
    }; // class StackUnderflow
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_STACK_UNDERFLOW_HPP
