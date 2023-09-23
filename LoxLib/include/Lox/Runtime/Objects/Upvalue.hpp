#ifndef LOX_VM_RUNTIME_OBJECTS_UP_VALUE_HPP
#define LOX_VM_RUNTIME_OBJECTS_UP_VALUE_HPP

#include "../Object.hpp"

#include "Lox/Runtime/Value.hpp"

namespace Lox
{
    // You can't implement this without using a pointer. (Effectively).
    
    class Upvalue final : public Object
    {
    public:
        Upvalue(GcPtr<Object> next, GcPtr<Upvalue> nextUpvalue, Value* slot);
        
        Value GetValue() const;
        
        void SetUpvalue(Value newValue);
        
        GcPtr<Upvalue> GetNextUpvalue();
        
        void SetNextUpvalue(GcPtr<Upvalue> upvalue);
        
        Value* GetSlot();
        
        void Close();
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        
        static ObjectType GetStaticType();
    
    private:
        GcPtr<Upvalue> nextUpvalue;
        
        Value* slot;
        Value closed;
        
        bool IsClosed() const;
        
        void MarkChildren() override;
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_UP_VALUE_HPP
