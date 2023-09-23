#ifndef LOX_VM_RUNTIME_OBJECT_TYPE_HPP
#define LOX_VM_RUNTIME_OBJECT_TYPE_HPP

#include <ostream>

#define LOX_OBJECT_TYPE_LIST(o) \
    o(String)                   \
    o(Closure)                  \
    o(Native)                   \
    o(Upvalue)                  \
    o(Class)                    \
    o(Instance)                 \
    o(BoundMethod)

namespace Lox
{
    enum class ObjectType
    {
        #define LOX_OBJECT_TYPE_ENUM(name) name,
        
        LOX_OBJECT_TYPE_LIST(LOX_OBJECT_TYPE_ENUM)
        
        #undef LOX_OBJECT_TYPE_ENUM
    };
    
    std::ostream& operator<<(std::ostream& out, ObjectType type);
}

#endif // LOX_VM_RUNTIME_OBJECT_TYPE_HPP
