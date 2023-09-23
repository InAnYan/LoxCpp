#ifndef LOX_VM_RUNTIME_OBJECTS_INSTANCE_HPP
#define LOX_VM_RUNTIME_OBJECTS_INSTANCE_HPP

#include <optional>

#include "Lox/Runtime/Object.hpp"
#include "Lox/Runtime/Value.hpp"

#include "Class.hpp"

namespace Lox
{
    class Instance : public Object
    {
    public:
        Instance(GcPtr<Object> next, GcPtr<Class> klass);
        
        GcPtr<const Class> GetClass() const;
        
        GcPtr<Class> GetClass();
        
        std::optional<Value> GetField(GcPtr<String> str);
        
        void SetField(GcPtr<String> str, Value newVal);
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        
        static ObjectType GetStaticType();
    
    private:
        GcPtr<Class> klass;
        std::unordered_map<GcPtr<String>, Value> fields;
        
        void MarkChildren() override;
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_INSTANCE_HPP
