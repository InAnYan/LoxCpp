#ifndef LOX_VM_RUNTIME_OBJECTS_BOUND_METHOD_HPP
#define LOX_VM_RUNTIME_OBJECTS_BOUND_METHOD_HPP

#include "../Object.hpp"

#include "../Value.hpp"

#include "Closure.hpp"
#include "Instance.hpp"

namespace Lox
{
    class BoundMethod : public Object
    {
    public:
        BoundMethod(GcPtr<Object> next, GcPtr<Instance> reciever, GcPtr<Closure> method);
        
        GcPtr<Instance> GetReciever();
        
        GcPtr<Closure> GetMethod();
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        
        static ObjectType GetStaticType();
    
    private:
        GcPtr<Instance> reciever;
        GcPtr<Closure> method;
        
        void MarkChildren() override;
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_BOUND_METHOD_HPP
