#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_NONCALLABLE_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_NONCALLABLE_HPP

#include "RuntimeException.hpp"

#include "Lox/Runtime/Value.hpp"

namespace Lox::Exceptions
{
    class NonCallable : public RuntimeException
    {
    public:
        NonCallable(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, Value val);
        
        Value GetValue() const;
        
        std::ostream& PrintAdditional(std::ostream& out) const override;
        
    private:
        Value val;
    };
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_NONCALLABLE_HPP
