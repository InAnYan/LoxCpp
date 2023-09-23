#include "Lox/Interpreter/Exceptions/InvalidStackAccess.hpp"

namespace Lox::Exceptions
{
    InvalidStackAccess::InvalidStackAccess(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace)
            : RuntimeException(pos, std::move(stackTrace), "invalid stack access")
    {
        
    }
}
