#include "Lox/Runtime/Objects/Native.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

namespace Lox
{
    Native::Native(GcPtr<Object> next, GcPtr<String> name, std::size_t arity, NativeFn fn)
            : Object(next, ObjectType::Native), name(name), arity(arity), fn(fn)
    {
    
    }
    
    GcPtr<String> Native::GetName()
    {
        return name;
    }
    
    GcPtr<const String> Native::GetName() const
    {
        return name;
    }
    
    std::size_t Native::GetArity() const
    {
        return arity;
    }
    
    NativeFn& Native::GetNativeFn()
    {
        return fn;
    }
    
    const NativeFn& Native::GetNativeFn() const
    {
        return fn;
    }
    
    std::ostream& Native::Print(std::ostream& out, PrintFlags flags) const
    {
        return out << "<native " << name->GetCppString() << ">";
    }
    
    void Native::MarkChildren()
    {
        MemoryManager::MarkObject(name);
    }
    
    ObjectType Native::GetStaticType()
    {
        return ObjectType::Native;
    }
}
