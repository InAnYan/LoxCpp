#include "Lox/Runtime/ValueType.hpp"

namespace Lox
{
    std::ostream& operator<<(std::ostream& out, ValueType type)
    {
        switch (type)
        {
            #define LOX_VALUE_TYPE_PRINT(name, _) \
                case ValueType::name: out << #name; break;
        
            
            LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_PRINT);
            
            #undef LOX_VALUE_TYPE_PRINT
            
            // TODO: Add hashes here.
        default: out << "Unknown"; break;
        }
        
        return out;
    }

    std::size_t ValueTypeToNumber(ValueType type)
    {
        return static_cast<std::size_t>(type);
    }
}
