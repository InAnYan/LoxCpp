#ifndef LOX_VM_RUNTIME_OBJECTS_CLASS_HPP
#define LOX_VM_RUNTIME_OBJECTS_CLASS_HPP

#include <optional>

#include "../Object.hpp"

#include "String.hpp"
#include "Closure.hpp"

namespace Lox
{
    class Class final : public Object
    {
    public:
        Class(GcPtr<Object> next, GcPtr<String> name);
        
        GcPtr<const String> GetName() const;
        
        GcPtr<String> GetName();
        
        void AddMethod(GcPtr<String> methodName, GcPtr<Closure> func);
        
        std::optional<GcPtr<Closure>> GetMethod(GcPtr<String> methodName);
        
        void Inherit(GcPtr<Class> other);
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        
        static ObjectType GetStaticType();
    
    private:
        GcPtr<String> name;
        std::unordered_map<GcPtr<String>, GcPtr<Closure>> methods;
        
        void MarkChildren() override;
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_CLASS_HPP
