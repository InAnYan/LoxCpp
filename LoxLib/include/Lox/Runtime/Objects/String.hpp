#ifndef LOX_VM_RUNTIME_OBJECTS_STRING_HPP
#define LOX_VM_RUNTIME_OBJECTS_STRING_HPP

#include <string>
#include <functional>

#include "Lox/Runtime/Object.hpp"

namespace Lox
{
    class String final : public Object
    {
    public:
        // Reevaluation of the hash.
        String(GcPtr<Object> next, std::string&& movedStr);
        
        const std::string& GetCppString() const;
        
        std::size_t GetHash() const;
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        // bool FullEqualTo(const Object& other) const override;
        
        static ObjectType GetStaticType();
    
    private:
        std::string str;
        std::size_t hash;
        
        void MarkChildren() override;
    };
}

namespace std
{
    template <>
    struct hash<Lox::String>
    {
        std::size_t operator()(const Lox::String& str) const
        {
            return str.GetHash();
        }
    };
    
    template <>
    struct hash<Lox::GcPtr<Lox::String>>
    {
        std::size_t operator()(Lox::GcPtr<Lox::String> str) const
        {
            return str->GetHash();
        }
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_STRING_HPP
