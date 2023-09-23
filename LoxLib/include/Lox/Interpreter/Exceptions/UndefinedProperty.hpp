#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_UNDEFINED_PROPERTY_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_UNDEFINED_PROPERTY_HPP

#include "RuntimeException.hpp"

#include "Lox/Runtime/Objects/String.hpp"

namespace Lox::Exceptions
{
    class UndefinedProperty : public RuntimeException
    {
    public:
        UndefinedProperty(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, GcPtr<String> name);
        
        GcPtr<const String> GetName() const;
        
        std::ostream& PrintAdditional(std::ostream& out) const override;
        
    private:
        GcPtr<String> name;
    };
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_UNDEFINED_PROPERTY_HPP