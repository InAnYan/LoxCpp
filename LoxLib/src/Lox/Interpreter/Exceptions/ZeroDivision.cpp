#include "Lox/Interpreter/Exceptions/ZeroDivision.hpp"

namespace Lox
{
    namespace Exceptions
    {
        ZeroDivision::ZeroDivision(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace)
            : RuntimeException(pos, std::move(stackTrace), "zero division")
        {

        }
        }
}
