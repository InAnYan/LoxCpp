#ifndef LOX_VM_RUNTIME_OBJECTS_FUNCTION_HPP
#define LOX_VM_RUNTIME_OBJECTS_FUNCTION_HPP

#include <vector>

#include "Lox/Runtime/Object.hpp"

#include "Lox/Runtime/Objects/String.hpp"
#include "Lox/Runtime/Objects/Upvalue.hpp"

#include "Lox/DataStructures/Chunk.hpp"

namespace Lox
{
    class Closure final : public Object
    {
    public:
        Closure(GcPtr<Object> next, GcPtr<String> name, std::size_t arity, Chunk&& chunk = Chunk());
        
        GcPtr<String> GetName();
        
        GcPtr<const String> GetName() const;
        
        std::size_t GetArity() const;
        
        // Ugly. Ugly. Ugly. Ugly. Ugly. Ugly. Ugly. Ugly.
        // God, forgive me, that I wrote this function.
        // It's because I don't have AST and I have to add
        // parameters to function while not knowing the whole count.
        void UglyChangeArity(std::size_t to);
        
        const Chunk& GetChunk() const;
        
        Chunk& GetChunk();
        
        void AddUpValue(GcPtr<Upvalue> upvalue);
        
        std::size_t GetUpvalueCount() const;
        
        GcPtr<Upvalue> GetUpvalue(std::size_t index);
        
        bool HasUpvalue(std::size_t index) const;
        
        std::ostream& Print(std::ostream& out, PrintFlags flags) const override;
        
        static ObjectType GetStaticType();
    
    private:
        GcPtr<String> name;
        std::size_t arity;
        Chunk chunk;
        std::vector<GcPtr<Upvalue>> upvalues;
        
        void MarkChildren() override;
    };
}

#endif // LOX_VM_RUNTIME_OBJECTS_FUNCTION_HPP
