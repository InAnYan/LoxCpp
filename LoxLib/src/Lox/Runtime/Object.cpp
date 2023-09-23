#include "Lox/Runtime/Object.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

namespace Lox
{
    Object::Object(GcPtr<Lox::Object> next, Lox::ObjectType type)
            : marked(false), next(next), type(type)
    {
    
    }
    
    ObjectType Object::GetType() const
    {
        return type;
    }
    
    bool Object::Is(ObjectType otherType) const
    {
        return type == otherType;
    }
    
    GcPtr<Object> Object::GetNext()
    {
        return next;
    }
    
    void Object::SetNext(GcPtr<Object> newNext)
    {
        next = newNext;
    }
    
    void Object::SetMarked()
    {
        marked = true;
    }
    
    void Object::Unmark()
    {
        marked = false;
    }
    
    bool Object::IsMarked() const
    {
        return marked;
    }
    
    std::ostream& operator<<(std::ostream& out, const Object& obj)
    {
        return obj.Print(out, PrintFlags::Raw);
    }
}