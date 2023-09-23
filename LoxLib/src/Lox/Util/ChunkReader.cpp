#include "Lox/Util/ChunkReader.hpp"

#include <algorithm>
#include <exception>
#include <cstring>
#include <memory>

#include "Lox/Util/ChunkSerialization.hpp"

#include "Lox/Runtime/Objects/String.hpp"

namespace Lox
{
    class ReadError : public std::exception
    {

    }; // class ReadError

    std::size_t ReadSize(std::istream& in);
    std::string ReadString(std::istream& in);

    class ChunkReader
    {
    public:
        ChunkReader(VirtualMachine& vm, std::istream& in, Chunk& chunk);
            
        bool Read();

    private:
        VirtualMachine& vm;
        
        std::istream& in;
        Chunk& chunk;

        void ReadRaw();

        void ReadCode();
        void ReadConstants();

        Value ReadValue();
        GcPtr<Object> ReadObject();

        template <typename T>
        T ReadData(std::size_t size);
    }; // class ChunkReader

    std::unique_ptr<Chunk> ReadChunk(VirtualMachine& vm, std::istream& in)
    {
        try
        {
            std::unique_ptr<Chunk> chunk = std::make_unique<Chunk>();

            ChunkReader reader(vm, in, *chunk);
            if (!reader.Read())
            {
                return nullptr;
            }
            else
            {
                return chunk;
            }
        }
        catch (const ReadError& e)
        {
            return nullptr;
        }
    }

    ChunkReader::ChunkReader(VirtualMachine& vm, std::istream& in, Chunk& chunk)
        : vm(vm), in(in), chunk(chunk)
    {
            
    }
            
    bool ChunkReader::Read()
    {
        try
        {
            ReadRaw();
            return true;
        }
        catch (const ReadError& e)
        {
            return false;
        }
    }

    void ChunkReader::ReadRaw()
    {
        ReadCode();
        ReadConstants();
    }

    void ChunkReader::ReadCode()
    {
        std::size_t size = ReadSize(in);

        for (std::size_t i = 0; i < size; ++i)
        {
            uint8_t byte = in.get();
            auto line = ReadData<std::size_t>(ChunkInfo::SizeOfSize);

            chunk.PushCode(byte, line);
        }
    }

    void ChunkReader::ReadConstants()
    {
        std::size_t size = ReadSize(in);

        for (std::size_t i = 0 ; i < size; ++i)
        {
            Value val = ReadValue();
            auto line = ReadData<std::size_t>(ChunkInfo::SizeOfSize);
                
            chunk.PushConstant(val, line);
        }
    }

    Value ChunkReader::ReadValue()
    {
        auto mark = ReadData<uint64_t>(ChunkInfo::TypeMarkSize);
        auto type = static_cast<ValueType>(mark);

        switch (type)
        {
        case ValueType::Nil:
            return NIL;
        case ValueType::Bool:
        {
            auto var = ReadData<bool>(ChunkInfo::ValueSize);
            return Value(var);
        }
        case ValueType::Double:
        {
            auto num = ReadData<double>(ChunkInfo::ValueSize);
            return Value(num);
        }
        case ValueType::Object:
            return Value(ReadObject());
        default:
            throw ReadError();
        }
    }
    
    GcPtr<Object> ChunkReader::ReadObject()
    {
        auto mark = ReadData<uint64_t>(ChunkInfo::TypeMarkSize);
        auto type = static_cast<ObjectType>(mark);
        
        switch (type)
        {
        case ObjectType::String:
        {
            return vm.InternStringTake(ReadString(in));
        }
        default:
            throw ReadError();
        }
    }

    template <typename T>
    T ChunkReader::ReadData(std::size_t size)
    {
        T obj;
        in.read(reinterpret_cast<char*>(&obj), size);
        return obj;
    }

    std::size_t ReadSize(std::istream& in)
    {
        std::size_t size;

        in.read(reinterpret_cast<char*>(&size), ChunkInfo::SizeOfSize);

        if (in.fail())
        {
            throw ReadError();
        }

        return size;
    }

    std::string ReadString(std::istream& in)
    {
        std::size_t size = ReadSize(in);

        std::string str(size, '\0');
        in.read(str.data(), size);

        if (in.fail())
        {
            throw ReadError();
        }

        return str;
    }
}
