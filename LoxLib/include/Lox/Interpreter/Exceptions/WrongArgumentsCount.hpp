// Have you ever seen such a long header guard?
#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_WRONG_ARGUMENTS_COUNT_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_WRONG_ARGUMENTS_COUNT_HPP

#include <cstdint>

#include "RuntimeException.hpp"

namespace Lox::Exceptions
{
    class WrongArgumentsCount : public RuntimeException
    {
    public:
        WrongArgumentsCount(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, uint8_t expected,
                            uint8_t got);
        
        uint8_t GetExpected() const;
        
        uint8_t GetGot() const;
        
        std::ostream& PrintAdditional(std::ostream& out) const override;
    
    private:
        uint8_t expected;
        uint8_t got;
    };
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_WRONG_ARGUMENTS_COUNT_HPP
