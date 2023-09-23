#include "Lox/Runtime/Objects/Instance.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

namespace Lox
{
    Instance::Instance(GcPtr<Object> next, GcPtr<Class> klass)
            : Object(next, ObjectType::Instance), klass(klass)
    {
    
    }
    
    GcPtr<const Class> Instance::GetClass() const
    {
        return klass;
    }
    
    GcPtr<Class> Instance::GetClass()
    {
        return klass;
    }
    
    std::optional<Value> Instance::GetField(GcPtr<String> str)
    {
        auto it = fields.find(str);
        
        if (it == fields.end())
        {
            return std::nullopt;
        }
        
        return it->second;
    }
    
    void Instance::SetField(GcPtr<String> str, Value newVal)
    {
        fields[str] = newVal;
    }
    
    std::ostream& Instance::Print(std::ostream& out, PrintFlags flags) const
    {
        return out << "<instance " << klass->GetName()->GetCppString() << ">";
    }
    
    ObjectType Instance::GetStaticType()
    {
        return ObjectType::Instance;
    }
    
    void Instance::MarkChildren()
    {
        MemoryManager::MarkObject(klass);
        
        for (auto& it : fields)
        {
            MemoryManager::MarkValue(it.second);
        }
    }
}