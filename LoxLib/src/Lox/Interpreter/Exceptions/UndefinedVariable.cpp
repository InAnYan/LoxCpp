#include "Lox/Interpreter/Exceptions/UndefinedVariable.hpp"

namespace Lox
{
    namespace Exceptions
    {
        UndefinedVariable::UndefinedVariable(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace,
                                             GcPtr<String> name)
                : RuntimeException(pos, std::move(stackTrace), "undefined variable"), name(name)
        {
        
        }
        
        GcPtr<String> UndefinedVariable::GetName() const
        {
            return name;
        }
        
        std::ostream& UndefinedVariable::PrintAdditional(std::ostream& out) const
        {
            return out << ": " << name->GetCppString();
        }
    }
}