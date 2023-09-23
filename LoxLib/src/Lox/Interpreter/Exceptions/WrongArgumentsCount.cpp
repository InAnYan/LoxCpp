#include "Lox/Interpreter/Exceptions/WrongArgumentsCount.hpp"

namespace Lox::Exceptions
{
    WrongArgumentsCount::WrongArgumentsCount(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace,
                                             uint8_t expected, uint8_t got)
            : RuntimeException(pos, std::move(stackTrace), "wrong arguments count"), expected(expected), got(got)
    {
    
    }
    
    uint8_t WrongArgumentsCount::GetExpected() const
    {
        return expected;
    }
    
    uint8_t WrongArgumentsCount::GetGot() const
    {
        return got;
    }
    
    std::ostream& WrongArgumentsCount::PrintAdditional(std::ostream& out) const
    {
        // Of course, `uint8_t` is not an `int`, but a `char`.
        return out << ": expected " << static_cast<int>(expected)
                   << ", got " << static_cast<int>(got);
    }
}