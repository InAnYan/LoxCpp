#ifndef LOX_VM_RUNTIME_OBJECTS_NATIVE_HPP
#define LOX_VM_RUNTIME_OBJECTS_NATIVE_HPP

#include <functional>

#include "Lox/Runtime/Object.hpp"
#include "Lox/Runtime/Value.hpp"

#include "Lox/Interpreter/StackIterator.hpp"

#include "String.hpp"

namespace Lox
{
    class VirtualMachine;
    
    using NativeFn = std::function<Value(VirtualMachine& vm, StackIterator args)>;
    
    class Native final : public Object
    {
    public:
        Native(GcPtr<Object> next, GcPtr<String> name, std::size_t arity, NativeFn fn);
        
        GcPtr<String> GetName();
        
        GcPtr<const String> GetName() const;
        
        std::size_t GetArity() const;
        
        NativeFn& GetNativeFn();
        
        const NativeFn& GetNativeFn() const;
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        
        static ObjectType GetStaticType();
    
    private:
        GcPtr<String> name;
        std::size_t arity;
        NativeFn fn;
        
        void MarkChildren() override;
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_NATIVE_HPP
