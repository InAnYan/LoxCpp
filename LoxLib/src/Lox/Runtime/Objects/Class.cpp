#include "Lox/Runtime/Objects/Class.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

#include "Lox/Interpreter/Exceptions/UndefinedProperty.hpp"

namespace Lox
{
    Class::Class(GcPtr<Object> next, GcPtr<String> name)
            : Object(next, ObjectType::Class), name(name)
    {
    
    }
    
    GcPtr<const String> Class::GetName() const
    {
        return name;
    }
    
    GcPtr<String> Class::GetName()
    {
        return name;
    }
    
    void Class::AddMethod(GcPtr<String> methodName, GcPtr<Closure> func)
    {
        methods[methodName] = func;
    }
    
    std::optional<GcPtr<Closure>> Class::GetMethod(GcPtr<String> methodName)
    {
        auto it = methods.find(methodName);
        
        if (it == methods.end())
        {
            return std::nullopt;
        }
        
        return it->second;
    }

void Class::Inherit(GcPtr<Class> other)
{
    methods.insert(other->methods.begin(), other->methods.end());
}
    
    std::ostream& Class::Print(std::ostream& out, PrintFlags flags) const
    {
        return out << "<class " << name->GetCppString() << ">";
    }
    
    void Class::MarkChildren()
    {
        MemoryManager::MarkObject(name);
    }
    
    ObjectType Class::GetStaticType()
    {
        return ObjectType::Class;
    }
}