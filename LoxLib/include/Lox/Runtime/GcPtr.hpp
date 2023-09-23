#ifndef LOX_VM_RUNTIME_GC_PTR_HPP
#define LOX_VM_RUNTIME_GC_PTR_HPP

#include <type_traits>

#include "Lox/Util/Assert.hpp"

namespace Lox
{
    /// A template type that signs that an object is managed by GC (aka `MemoryManager`).
    template <typename T>
    class GcPtr
    {
    public:
        GcPtr()
                : ptr(nullptr)
        {
        
        }
        
        explicit GcPtr(T* obj)
                : ptr(obj)
        {
        
        }
        
        GcPtr(const GcPtr<T>& other) = default;
        
        ~GcPtr() = default;
        
        bool IsNullptr() const
        {
            return ptr == nullptr;
        }
        
        T* operator->()
        {
            LOX_ASSERT(!IsNullptr(), "dereferencing nullptr Lox::GcPtr");
            return ptr;
        }
        
        const T* operator->() const
        {
            LOX_ASSERT(!IsNullptr(), "dereferencing nullptr Lox::GcPtr");
            return ptr;
        }
        
        T& operator*()
        {
            LOX_ASSERT(!IsNullptr(), "dereferencing nullptr Lox::GcPtr");
            return *ptr;
        }
        
        const T& operator*() const
        {
            LOX_ASSERT(!IsNullptr(), "dereferencing nullptr Lox::GcPtr");
            return *ptr;
        }
        
        template <typename U>
        operator GcPtr<U>()
        {
            static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
                          "invalid cast for Lox::GcPtr, types should be in inheritance");
            return GcPtr<U>(reinterpret_cast<U*>(ptr));
        }
        
        template <typename U>
        operator GcPtr<const U>() const
        {
            static_assert(std::is_base_of<T, U>::value || std::is_base_of<U, T>::value,
                          "invalid cast for Lox::GcPtr, types should be in inheritance");
            return GcPtr<const U>(reinterpret_cast<const U*>(ptr));
        }
        
        template <typename Return, typename... Args>
        Return operator()(Args&& ... args) const
        {
            return (*ptr)(std::forward<Args&&>(args)...);
        }
        
        std::size_t Hash() const
        {
            return std::hash<T*>{}(ptr);
        }
        
        bool operator==(const GcPtr<T>& other) const
        {
            return this->ptr == other.ptr;
        }
        
        bool operator!=(const GcPtr<T>& other) const
        {
            return !operator==(other);
        }
        
        operator bool() const
        {
            return !IsNullptr();
        }
        
        void Delete() const
        {
            delete ptr;
        }
        
        std::ostream& PrintPointerValue(std::ostream& out) const
        {
            return out << ptr;
        }
    
    private:
        T* ptr;
    }; // class GcPtr
    
    template <typename T>
    inline std::ostream& operator<<(std::ostream& out, const GcPtr<T>& gcPtr)
    {
        LOX_ASSERT(!gcPtr.IsNullptr(), "dereferencing nullptr Lox::GcPtr");
        return out << *gcPtr;
    }
}

#endif // LOX_VM_RUNTIME_GC_PTR_HPP
