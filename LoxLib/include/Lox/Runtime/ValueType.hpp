#ifndef LOX_VM_RUNTIME_VALUE_TYPE_HPP
#define LOX_VM_RUNTIME_VALUE_TYPE_HPP

#include <cstddef>
#include <ostream>

#include "Lox/Util/Assert.hpp"

#include "Nil.hpp"
#include "GcPtr.hpp"
#include "Object.hpp"

#define LOX_VALUE_TYPE_LIST(o)    \
    o(Nil, Nil)                   \
    o(Bool, bool)                 \
    o(Double, double)             \
    o(Object, GcPtr<Object>)

namespace Lox
{
    enum class ValueType
    {
        #define LOX_VALUE_TYPE_ENUM(name, type) \
            name,
        
        LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_ENUM)
        
        #undef LOX_VALUE_TYPE_ENUM
    }; // enum class ValueType
    
    std::ostream& operator<<(std::ostream& out, ValueType type);
    
    template <typename T>
    inline ValueType TypeToValueType()
    {
        LOX_FAIL("type is not ValueType");
        return static_cast<ValueType>(0);
    }
    
    #define LOX_TYPE_TO_VALUE_TYPE_O(name, type)       \
        template <>                                    \
        inline ValueType TypeToValueType<type>()       \
        {                                              \
            return ValueType::name;                    \
        }
    
    LOX_VALUE_TYPE_LIST(LOX_TYPE_TO_VALUE_TYPE_O)
    
    #undef LOX_TYPE_TO_VALUE_TYPE_O
    
    #define LOX_VALUE_TYPE_USING(name, type) \
        using name##Repr = type;
    
    LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_USING);
    
    #undef LOX_VALUE_TYPE_USING
}

#endif // LOX_VM_RUNTIME_VALUE_TYPE_HPP
