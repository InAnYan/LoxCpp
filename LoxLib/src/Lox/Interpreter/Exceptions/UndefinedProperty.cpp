#include "Lox/Interpreter/Exceptions/UndefinedProperty.hpp"

namespace Lox::Exceptions
{
    UndefinedProperty::UndefinedProperty(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace,
                                         GcPtr<String> name)
            : RuntimeException(pos, std::move(stackTrace), "undefined property"), name(name)
    {
    
    }
    
    GcPtr<const String> UndefinedProperty::GetName() const
    {
        return name;
    }
    
    std::ostream& UndefinedProperty::PrintAdditional(std::ostream& out) const
    {
        return out << ": " << name->GetCppString();
    }
}