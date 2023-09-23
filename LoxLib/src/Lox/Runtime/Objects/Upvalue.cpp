#include "Lox/Runtime/Objects/Upvalue.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

namespace Lox
{
    Upvalue::Upvalue(GcPtr<Object> next, GcPtr<Upvalue> nextUpvalue, Value* slot)
            : Object(next, ObjectType::Upvalue), nextUpvalue(nextUpvalue), slot(slot), closed(NIL)
    {
    
    }
    
    Value Upvalue::GetValue() const
    {
        return *slot;
    }
    
    void Upvalue::SetUpvalue(Value newValue)
    {
        *slot = newValue;
    }
    
    GcPtr<Upvalue> Upvalue::GetNextUpvalue()
    {
        LOX_ASSERT(!IsClosed(), "Lox::Upvalue getting next upvalue of closed upvalue");
        return nextUpvalue;
    }
    
    void Upvalue::SetNextUpvalue(GcPtr<Upvalue> upvalue)
    {
        LOX_ASSERT(!IsClosed(), "Lox::Upvalue getting next upvalue of closed upvalue");
        nextUpvalue = upvalue;
    }
    
    Value* Upvalue::GetSlot()
    {
        LOX_ASSERT(!IsClosed(), "Lox::Upvalue getting stack slot of closed upvalue");
        return slot;
    }
    
    void Upvalue::Close()
    {
        closed = *slot;
        slot = &closed;
    }
    
    std::ostream& Upvalue::Print(std::ostream& out, PrintFlags flags) const
    {
        return out << "<upvalue>";
    }
    
    bool Upvalue::IsClosed() const
    {
        return slot == &closed;
    }
    
    void Upvalue::MarkChildren()
    {
        // NOTE: Marking of opened values is the VM responsibility.
        
        MemoryManager::MarkValue(closed);
    }
    
    ObjectType Upvalue::GetStaticType()
    {
        return ObjectType::Upvalue;
    }
}