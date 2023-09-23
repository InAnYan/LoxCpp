#ifndef LOX_VM_UTIL_CHUNK_READER_HPP
#define LOX_VM_UTIL_CHUNK_READER_HPP

#include <istream>
#include <memory>

#include "Lox/DataStructures/Chunk.hpp"

#include "Lox/Interpreter/VirtualMachine.hpp"

namespace Lox
{
    std::unique_ptr<Chunk> ReadChunk(VirtualMachine& vm, std::istream& in);
}

#endif // LOX_VM_UTIL_CHUNK_READER_HPP
