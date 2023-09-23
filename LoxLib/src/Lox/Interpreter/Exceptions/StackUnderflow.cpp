#include "Lox/Interpreter/Exceptions/StackUnderflow.hpp"

namespace Lox
{
    namespace Exceptions
    {
        StackUnderflow::StackUnderflow(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace)
                : RuntimeException(pos, std::move(stackTrace), "stack underflow")
        {
        
        }
    }
}
