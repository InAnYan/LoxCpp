#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_UNKNOWN_INSTRUCTION_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_UNKNOWN_INSTRUCTION_HPP

#include <cstdint>

#include "Lox/Interpreter/Exceptions/RuntimeException.hpp"

namespace Lox::Exceptions
{
    class UnknownInstruction final : public RuntimeException
    {
    public:
        UnknownInstruction(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, uint8_t byte);
        
        uint8_t GetByte() const;
        
        virtual std::ostream& PrintAdditional(std::ostream& out) const;
    
    private:
        uint8_t byte;
    }; // class UnknownInstruction
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_UNKNOWN_INSTRUCTION_HPP
