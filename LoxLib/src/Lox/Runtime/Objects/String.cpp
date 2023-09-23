#include "Lox/Runtime/Objects/String.hpp"

#include <iomanip>

namespace Lox
{
    String::String(GcPtr<Object> next, std::string&& movedStr)
            : Object(next, ObjectType::String), str(movedStr), hash(std::hash<std::string>{}(str))
    {
    
    }
    
    const std::string& String::GetCppString() const
    {
        return str;
    }
    
    std::size_t String::GetHash() const
    {
        return hash;
    }
    
    std::ostream& String::Print(std::ostream& out, PrintFlags flags) const
    {
        if (flags == PrintFlags::Raw)
        {
            // Wow, so that's why there is `std::quoted`.
            // It's also used to escape symbols!
            return out << std::quoted(str);
        }
        
        return out << str;
    }
    
    void String::MarkChildren()
    {
    
    }
    
    ObjectType String::GetStaticType()
    {
        return ObjectType::String;
    }
}