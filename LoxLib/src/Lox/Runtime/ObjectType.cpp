#include "Lox/Runtime/ObjectType.hpp"

namespace Lox
{
    std::ostream& operator<<(std::ostream& out, ObjectType type)
    {
        #define LOX_OBJECT_TYPE_PRINT(name) case ObjectType::name: out << #name; break;
        
        switch (type)
        {
        LOX_OBJECT_TYPE_LIST(LOX_OBJECT_TYPE_PRINT)
        default: out << "Unknown"; break;
        }
        
        #undef LOX_OBJECT_TYPE_PRINT
        
        return out;
    }
}