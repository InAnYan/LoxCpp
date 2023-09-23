#include "Lox/Runtime/Objects/Closure.hpp"

#include "Lox/Runtime/MemoryManager.hpp"

namespace Lox
{
    Closure::Closure(GcPtr<Object> next, GcPtr<String> name, std::size_t arity, Chunk&& chunk)
            : Object(next, ObjectType::Closure), name(name), arity(arity), chunk(chunk)
    {
    
    }
    
    void Closure::AddUpValue(GcPtr<Upvalue> upvalue)
    {
        upvalues.push_back(upvalue);
    }
    
    std::size_t Closure::GetUpvalueCount() const
    {
        return upvalues.size();
    }
    
    GcPtr<Upvalue> Closure::GetUpvalue(std::size_t index)
    {
        LOX_ASSERT(HasUpvalue(index), "Lox::Closure invalid upvalue index");
        return upvalues[index];
    }
    
    bool Closure::HasUpvalue(std::size_t index) const
    {
        return index < GetUpvalueCount();
    }
    
    GcPtr<String> Closure::GetName()
    {
        return name;
    }
    
    GcPtr<const String> Closure::GetName() const
    {
        return name;
    }
    
    std::size_t Closure::GetArity() const
    {
        return arity;
    }
    
    const Chunk& Closure::GetChunk() const
    {
        return chunk;
    }
    
    Chunk& Closure::GetChunk()
    {
        return chunk;
    }
    
    std::ostream& Closure::Print(std::ostream& out, PrintFlags flags) const
    {
        return out << "<fn " << name->GetCppString() << ">";
    }
    
    void Closure::MarkChildren()
    {
        MemoryManager::MarkObject(name);
        
        for (auto it = chunk.ConstantsBegin(); it != chunk.ConstantsEnd(); ++it)
        {
            MemoryManager::MarkValue(*it);
        }
        
        for (auto upvalue : upvalues)
        {
            MemoryManager::MarkObject(upvalue);
        }
    }
    
    ObjectType Closure::GetStaticType()
    {
        return ObjectType::Closure;
    }
    
    void Closure::UglyChangeArity(std::size_t to)
    {
        arity = to;
    }
}