#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_UNDEFINED_VARIABLE_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_UNDEFINED_VARIABLE_HPP

#include "RuntimeException.hpp"

#include "Lox/Runtime/GcPtr.hpp"
#include "Lox/Runtime/Objects/String.hpp"

namespace Lox::Exceptions
{
    class UndefinedVariable : public RuntimeException
    {
    public:
        UndefinedVariable(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, GcPtr<String> name);
        
        GcPtr<String> GetName() const;
        
        std::ostream& PrintAdditional(std::ostream& out) const override;
    
    private:
        GcPtr<String> name;
    };
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_UNDEFINED_VARIABLE_HPP
