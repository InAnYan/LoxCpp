#include "Lox/Interpreter/Exceptions/UnknownInstruction.hpp"

namespace Lox
{
    namespace Exceptions
    {
        UnknownInstruction::UnknownInstruction(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace,
                                               uint8_t byte)
                : RuntimeException(pos, std::move(stackTrace), "unknown instruction"), byte(byte)
        {
        
        }
        
        uint8_t UnknownInstruction::GetByte() const
        {
            return byte;
        }
        
        std::ostream& UnknownInstruction::PrintAdditional(std::ostream& out) const
        {
            return out << ": " << static_cast<int>(byte);
        }
    }
}
