#ifndef LOX_VM_UTIL_CHUNK_WRITER_HPP
#define LOX_VM_UTIL_CHUNK_WRITER_HPP

#include <ostream>
#include <string_view>

#include "Lox/DataStructures/Chunk.hpp"

#include "Lox/Util/ChunkSerialization.hpp"

namespace Lox
{
    // TODO: Signedness thing!!!
        
    class ChunkWriter
    {
    public:
        ChunkWriter(const Chunk& chunk, std::ostream& out);

        void Write();

    private:
        const Chunk& chunk;
        std::ostream& out;

        void WriteCode();
        void WriteConstants();

        void WriteSize(std::size_t size);
        void WriteValue(Value val);
        void WriteObject(GcPtr<Object> obj);

        template <typename T>
        void WriteData(const T& obj, std::size_t size);

        void WriteString(std::string_view str);
    }; // class ChunkWriter
}

#endif // LOX_VM_UTIL_CHUNK_WRITER_HPP
