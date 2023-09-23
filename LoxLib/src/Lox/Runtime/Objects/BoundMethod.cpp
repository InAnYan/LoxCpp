#include "Lox/Runtime/Objects/BoundMethod.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

namespace Lox
{
    BoundMethod::BoundMethod(GcPtr<Object> next, GcPtr<Instance> reciever, GcPtr<Closure> method)
            : Object(next, ObjectType::BoundMethod), reciever(reciever), method(method)
    {
    
    }
    
    GcPtr<Instance> BoundMethod::GetReciever()
    {
        return reciever;
    }
    
    GcPtr<Closure> BoundMethod::GetMethod()
    {
        return method;
    }
    
    std::ostream& BoundMethod::Print(std::ostream& out, PrintFlags flags) const
    {
        return out << "<bound-method " << method->GetName()->GetCppString() << " of " << reciever << ">";
    }
    
    ObjectType BoundMethod::GetStaticType()
    {
        return ObjectType::BoundMethod;
    }
    
    void BoundMethod::MarkChildren()
    {
        MemoryManager::MarkObject(reciever);
        MemoryManager::MarkObject(method);
    }
}