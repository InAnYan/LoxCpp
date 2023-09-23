#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_RUNTIME_EXCEPTION_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_RUNTIME_EXCEPTION_HPP

#include <exception>
#include <ostream>
#include <vector>
#include <utility>

#include "Lox/Compiler/SourcePosition.hpp"

#include "Lox/Runtime/GcPtr.hpp"
#include "Lox/Runtime/Objects/Closure.hpp"

namespace Lox::Exceptions
{
    using StackTraceEntry = std::pair<GcPtr<const Closure>, Chunk::CodeIterator>;
    
    class RuntimeException : public std::exception
    {
    public:
        RuntimeException(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, const char* msg);
        
        ~RuntimeException() override = default;
        
        const char* what() const noexcept override;
        
        const std::vector<StackTraceEntry>& GetStackTrace() const;
        
        SourcePosition GetPos() const;
        
        virtual std::ostream& PrintAdditional(std::ostream& out) const;
    
    private:
        SourcePosition pos;
        std::vector<StackTraceEntry> stackTrace;
        const char* msg;
    }; // class RuntimeException
    
    std::ostream& operator<<(std::ostream& out, const RuntimeException& e);
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_RUNTIME_EXCEPTION_HPP
