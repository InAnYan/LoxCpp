#include "Lox/Interpreter/CallFrame.hpp"

#include "Lox/Configuration.hpp"

#include "Lox/Util/Assert.hpp"
#include "Lox/Util/ChunkDumper.hpp"

namespace Lox
{
    CallFrame::CallFrame()
            : func(nullptr), values(nullptr), ip(nullptr)
    {
    
    }
    
    CallFrame::CallFrame(GcPtr<Closure> func, StackIterator values)
            : func(func), values(values), ip(func->GetChunk().CodeBegin())
    {
    
    }
    
    GcPtr<Closure> CallFrame::GetFunction()
    {
        return func;
    }
    
    GcPtr<const Closure> CallFrame::GetFunction() const
    {
        return func;
    }
    
    StackIterator CallFrame::GetValues() const
    {
        return values;
    }
    
    Chunk::CodeIterator CallFrame::GetIP() const
    {
        return ip;
    }
    
    uint8_t CallFrame::ReadByte()
    {
        // TODO: Is that a correct check?
        // Probably yes, if I only read opcodes in sequence, it is sufficient,
        // but when I have jumps I will check the jump correctness before changing the IP.
        
        LOX_ASSERT(ip != func->GetChunk().CodeEnd(), "Lox::CallFrame reading past the chunk");
        return *ip++;
    }
    
    uint16_t CallFrame::ReadShort()
    {
        uint8_t first = ReadByte();
        uint8_t second = ReadByte();
        
        return (first << 8) | second;
    }
    
    Value CallFrame::ReadConstant()
    {
        uint8_t byte = ReadByte();
        
        LOX_ASSERT(func->GetChunk().HasConstant(byte), "Lox::CallFrame reading wrong constant");
        
        return func->GetChunk().GetConstant(byte);
    }
    
    void CallFrame::Jump(int offset)
    {
        ip += offset;
    }
    
    bool CallFrame::IsAtEnd()
    {
        return ip == func->GetChunk().CodeEnd();
    }
    
    std::size_t CallFrame::GetPreviousLine() const
    {
        LOX_ASSERT(ip != func->GetChunk().CodeBegin(), "Lox::Record getting previous line from beginning");
        
        return func->GetChunk().GetCodeLine(ip - func->GetChunk().CodeBegin());
    }
    
    uint8_t CallFrame::GetPreviousByte() const
    {
        LOX_ASSERT(ip != func->GetChunk().CodeBegin(), "Lox::Record getting previous byte from beginning");
        
        return *(ip - 1);
    }
    
    SourcePosition CallFrame::MakeExceptionSourcePosition() const
    {
        return SourcePosition(GetPreviousLine());
    }
}
