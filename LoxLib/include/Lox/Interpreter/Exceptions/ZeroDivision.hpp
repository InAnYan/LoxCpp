#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_ZERO_DIVISION_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_ZERO_DIVISION_HPP

#include "Lox/Interpreter/Exceptions/RuntimeException.hpp"

namespace Lox::Exceptions
{
    class ZeroDivision : public RuntimeException
    {
    public:
        ZeroDivision(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace);
    }; // class ZeroDivision
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_ZERO_DIVISION_HPP
