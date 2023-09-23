#include "Lox/Interpreter/Exceptions/StackOverflow.hpp"

namespace Lox
{
    namespace Exceptions
    {
        StackOverflow::StackOverflow(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace)
                : RuntimeException(pos, std::move(stackTrace), "stack overflow")
        {
                
        }
    }
}
