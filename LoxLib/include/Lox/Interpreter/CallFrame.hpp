#ifndef LOX_VM_INTERPRETER_RECORD_HPP
#define LOX_VM_INTERPRETER_RECORD_HPP

#include "Lox/Configuration.hpp"

#include "Lox/Runtime/Value.hpp"

#include "Lox/Runtime/Objects/String.hpp"
#include "Lox/Runtime/Objects/Closure.hpp"

#include "Lox/DataStructures/Chunk.hpp"

#include "Lox/Compiler/SourcePosition.hpp"

#include "Opcode.hpp"
#include "StackIterator.hpp"

namespace Lox
{
    class CallFrame
    {
    public:
        CallFrame();
        
        CallFrame(GcPtr<Closure> func, StackIterator values);
        
        GcPtr<Closure> GetFunction();
        
        GcPtr<const Closure> GetFunction() const;
        
        StackIterator GetValues() const;
        
        Chunk::CodeIterator GetIP() const;
        
        uint8_t ReadByte();
        
        uint16_t ReadShort();
        
        Value ReadConstant();
        
        void Jump(int offset);
        
        bool IsAtEnd();
        
        std::size_t GetPreviousLine() const;
        
        uint8_t GetPreviousByte() const;
        
        SourcePosition MakeExceptionSourcePosition() const;
    
    private:
        GcPtr<Closure> func;
        StackIterator values;
        Chunk::CodeIterator ip;
    }; // class Record
}

#endif // LOX_VM_INTERPRETER_RECORD_HPP
