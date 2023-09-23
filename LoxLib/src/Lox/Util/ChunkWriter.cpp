#include "Lox/Util/ChunkWriter.hpp"

#include <type_traits>

#include "Lox/Runtime/Nil.hpp"
#include "Lox/Runtime/ValueType.hpp"

#include "Lox/Runtime/Objects/String.hpp"

#include "Lox/Util/ChunkSerialization.hpp"

namespace Lox
{
    ChunkWriter::ChunkWriter(const Chunk& chunk, std::ostream& out)
            : chunk(chunk), out(out)
    {
    
    }
    
    void ChunkWriter::Write()
    {
        WriteCode();
        WriteConstants();
    }
    
    void ChunkWriter::WriteCode()
    {
        WriteSize(chunk.GetCodeSize());
        
        for (std::size_t i = 0; i < chunk.GetCodeSize(); ++i)
        {
            WriteData(chunk.GetCode(i), 1);
            WriteData(chunk.GetCodeLine(i), ChunkInfo::LineSize);
        }
    }
    
    void ChunkWriter::WriteConstants()
    {
        WriteSize(chunk.GetConstantsCount());
        
        for (std::size_t i = 0; i < chunk.GetConstantsCount(); ++i)
        {
            WriteValue(chunk.GetConstant(i));
            WriteData(chunk.GetConstantLine(i), ChunkInfo::LineSize);
        }
    }
    
    void ChunkWriter::WriteSize(std::size_t size)
    {
        WriteData(size, ChunkInfo::SizeOfSize);
    }
    
    void ChunkWriter::WriteValue(Value val)
    {
        WriteData(static_cast<uint64_t>(val.GetType()), ChunkInfo::TypeMarkSize);
        
        val.Visit(overloaded
                          {
                                  [this](auto arg)
                                  {
                                      out.write(reinterpret_cast<const char*>(&arg), ChunkInfo::ValueSize);
                                  },
                                  [](Nil nil)
                                  {
                                  
                                  },
                                  [this](bool b)
                                  {
                                      uint64_t num = b ? 1 : 0;
                                      out.write(reinterpret_cast<const char*>(&num), ChunkInfo::ValueSize);
                                  },
                                  [this](GcPtr<Object> obj)
                                  {
                                      WriteObject(obj);
                                  }
                          });
    }
    
    void ChunkWriter::WriteObject(GcPtr<Object> obj)
    {
        WriteData(static_cast<uint64_t>(obj->GetType()), ChunkInfo::TypeMarkSize);
        
        switch (obj->GetType())
        {
        case ObjectType::String:
            WriteString(obj->As<String>()->GetCppString());
            break;
        default:
            LOX_UNREACHABLE("unimplemented object type writing");
        }
    }
    
    template <typename T>
    void ChunkWriter::WriteData(const T& obj, std::size_t size)
    {
        T var(obj);
        out.write(reinterpret_cast<const char*>(&var), size);
    }
    
    void ChunkWriter::WriteString(std::string_view str)
    {
        WriteSize(str.size());
        
        for (char ch: str)
        {
            out.put(ch);
        }
    }
}
