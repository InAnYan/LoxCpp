#include "Lox/Interpreter/Exceptions/NonCallable.hpp"

namespace Lox::Exceptions
{
    NonCallable::NonCallable(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, Value val)
            : RuntimeException(pos, std::move(stackTrace), "attempt to call non-callable value"), val(val)
    {
    
    }
    
    Value NonCallable::GetValue() const
    {
        return val;
    }
    
    std::ostream& NonCallable::PrintAdditional(std::ostream& out) const
    {
        return out << ": " << val;
    }
}