#ifndef LOX_VM_RUNTIME_VALUE_HPP
#define LOX_VM_RUNTIME_VALUE_HPP

#include <ostream>

#include "../Util/Assert.hpp"
#include "../Util/OverloadedVisitor.hpp"

#include "ValueType.hpp"

namespace Lox
{
    // At the beginning, I wrote value.AsDouble().
    // I really like this compiler, because it mirrors cast of objects.
    // But when I've added objects, there were some problems with type names,
    // and the only solution was to rename Object class into ObjectsRoot.
    // But I don't like that, so I decided to write value cast as
    // value.AsDouble(). That way I can change the implementation of Double value.
    
    // This class is so messy.
    
    class Value
    {
    public:
        template <typename T>
        explicit Value(T value)
                : type(TypeToValueType<T>()), as(value)
        {
        
        }
        
        template <typename T>
        explicit Value(GcPtr<T> object)
                : type(ValueType::Object), as(object)
        {
        
        }
        
        // Yeah, there will be a default constructor.
        Value()
                : Value(Nil())
        {
        
        }
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const;
        
        /// Don't use this method in interpreter's code.
        template <typename T>
        T As() const
        {
            LOX_ASSERT(Is<T>(), "Lox::Value bad cast");
            return as.Get<T>();
        }
        
        /// Don't use this method in interpreter's code.
        template <typename T>
        bool Is() const
        {
            return type == TypeToValueType<T>();
        }
        
        #define LOX_VALUE_TYPE_CAST(name, type)                 \
            type As##name() const                               \
            {                                                   \
                LOX_ASSERT(Is##name(), "Lox::Value bad cast");  \
                return as.field_##name;                         \
            }
        
        LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_CAST);
        
        #undef LOX_VALUE_TYPE_CAST
        
        template <typename T>
        GcPtr<T> AsObject() const
        {
            LOX_ASSERT(IsObject() && AsObject()->Is<T>(), "Lox::Value bad cast");
            return AsObject()->As<T>();
        }
        
        bool Is(ValueType otherType) const
        {
            return this->type == otherType;
        }
        
        #define LOX_VALUE_TYPE_IS(name, type) \
            bool Is##name() const             \
            {                                 \
                return Is(ValueType::name);   \
            }
        
        LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_IS);
        
        #undef LOX_VALUE_TYPE_IS
        
        template <typename T>
        bool IsObject() const
        {
            return IsObject() && AsObject()->Is<T>();
        }
        
        bool IsObject(ObjectType otherType) const
        {
            return IsObject() && AsObject()->Is(otherType);
        }
        
        template <class Visitor>
        void Visit(Visitor&& visitor) const
        {
            switch (type)
            {
                #define LOX_VALUE_TYPE_VISIT(name, type) \
                    case ValueType::name: visitor(as.field_##name); break;
            
            LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_VISIT);
                
                #undef LOX_VALUE_TYPE_VISIT
            }
        }
        
        ValueType GetType() const;
        
        bool operator==(const Value& value) const;
        
        bool operator!=(const Value& value) const;
        
        bool IsTrue() const;
        
        bool IsFalse() const;
    
    private:
        ValueType type;
        
        union As
        {
            #define LOX_VALUE_TYPE_UNION_AS(name, type) \
                As(type val) : field_##name(val) {}
            
            template <typename T>
            As(GcPtr<T> obj) : field_Object(obj)
            {
            }
            
            LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_UNION_AS)
            
            #undef LOX_VALUE_TYPE_UNION_AS
            
            #define LOX_VALUE_TYPE_UNION_FIELD(name, type) \
                type field_##name;
            
            LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_UNION_FIELD)
            
            #undef LOX_VALUE_TYPE_UNION_FIELD
            
            template <typename T>
            T Get() const
            {
                LOX_FAIL("wrong Value::As get");
            }
            
            #define LOX_VALUE_TYPE_UNION_GET(name, type)      \
                template <>                                   \
                type Get<type>() const                        \
                {                                             \
                    return field_##name;                      \
                }
            
            LOX_VALUE_TYPE_LIST(LOX_VALUE_TYPE_UNION_GET)
            
            #undef LOX_VALUE_TYPE_UNION_GET
        } as;
    }; // class Value
    
    extern Value NIL;
    
    std::ostream& operator<<(std::ostream& out, const Value& value);
}

#endif // LOX_VM_RUNTIME_VALUE_HPP
