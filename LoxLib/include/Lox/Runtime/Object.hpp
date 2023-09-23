#ifndef LOX_VM_RUNTIME_OBJECT_HPP
#define LOX_VM_RUNTIME_OBJECT_HPP

#include <ostream>

#include "ObjectType.hpp"
#include "GcPtr.hpp"
#include "PrintFlags.hpp"

namespace Lox
{
    class Object
    {
    public:
        virtual ~Object() = default;
        
        virtual std::ostream& Print(std::ostream& out, PrintFlags flags) const = 0;
        // virtual bool FullEqualTo(const Object& other) const = 0;
        
        template <typename T>
        static constexpr bool IsValidLoxObject()
        {
            static_assert(std::is_base_of<Object, T>::value && "the type should inherit from Lox::Object");
            static_assert(std::is_same<decltype(T::GetStaticType()), ObjectType>::value,
                          "the type should implement a static method Lox::ObjectType GetStaticType()");
            
            return true;
        }
        
        ObjectType GetType() const;
        
        bool Is(ObjectType otherType) const;
        
        template <typename T>
        bool Is() const
        {
            static_assert(IsValidLoxObject<T>() && "the type is not an Lox::Object");
            return Is(T::GetStaticType());
        }
        
        // NOTE: Bad design decision.
        // 1. GC code outside MemoryManager.
        // 2. What if the objects is not managed? Then `As` should not be `GcPtr`.
        // I left it, because it is convenient. Nowhere we create objects outside of MemoryManager.
        template <typename T>
        GcPtr<T> As()
        {
            // This template is so cool. No need to create AsString, AsSymbol and so on.
            // And with GetStaticType we can even check for correctness.
            static_assert(IsValidLoxObject<T>() && "the type is not an Lox::Object");
            LOX_ASSERT(GetType() == T::GetStaticType(), "bad cast for Lox::Object");
            return GcPtr<T>(reinterpret_cast<T*>(this));
        }
        
        template <typename T>
        GcPtr<const T> As() const
        {
            // This template is so cool. No need to create AsString, AsSymbol and so on.
            // And with GetStaticType we can even check for correctness.
            static_assert(IsValidLoxObject<T>() && "the type is not an Lox::Object");
            LOX_ASSERT(GetType() == T::GetStaticType(), "bad cast for Lox::Object");
            return GcPtr<const T>(reinterpret_cast<const T*>(this));
        }
        
        // Methods that are used by GC.
        
        GcPtr<Object> GetNext();
        
        void SetNext(GcPtr<Object> newNext);
        
        void SetMarked(); // Does not call `MarkChildren`.
        void Unmark();
        
        bool IsMarked() const;
        
        virtual void MarkChildren() = 0;
    
    protected:
        Object(GcPtr<Object> next, ObjectType type);
    
    private:
        bool marked;
        ObjectType type;
        GcPtr<Object> next;
    };
    
    std::ostream& operator<<(std::ostream& out, const Object& obj);
}

#endif // LOX_VM_RUNTIME_OBJECT_HPP
