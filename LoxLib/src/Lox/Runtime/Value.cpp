#include "Lox/Runtime/Value.hpp"

#include <type_traits>

namespace Lox
{
    Value NIL = Value(Nil());
    
    ValueType Value::GetType() const
    {
        return type;
    }
    
    bool Value::operator==(const Value& other) const
    {
        if (!Is(other.GetType()))
        {
            return false;
        }
        
        switch (type)
        {
            #define LOX_VALUE_TYPE_EQ(name, type) \
                case ValueType::name: return this->as.field_##name == other.as.field_##name;
        
        LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_EQ)
            
            #undef LOX_VALUE_TYPE_EQ
        }
    }
    
    bool Value::operator!=(const Value& value) const
    {
        return !operator==(value);
    }
    
    bool Value::IsTrue() const
    {
        return !IsFalse();
    }
    
    bool Value::IsFalse() const
    {
        return IsNil() || (IsBool() && !AsBool());
    }
    
    std::ostream& Value::Print(std::ostream& out, PrintFlags flags) const
    {
        Visit([&out, flags](auto arg)
              {
                  using Type = decltype(arg);
                  if constexpr (std::is_same_v<Type, bool>)
                  {
                      out << (arg ? "true" : "false");
                  }
                  else if constexpr (std::is_same_v<Type, GcPtr<Object>>)
                  {
                      arg->Print(out, flags);
                  }
                  else
                  {
                      out << arg;
                  }
              });
        
        return out;
    }
    
    std::ostream& operator<<(std::ostream& out, const Value& value)
    {
        return value.Print(out, PrintFlags::Raw);
    }
}
