#ifndef LOX_VM_UTIL_CHUNK_DUMPER_HPP
#define LOX_VM_UTIL_CHUNK_DUMPER_HPP

#include <cstdint>
#include <iostream>

#include "Lox/DataStructures/Chunk.hpp"

#include "Lox/Interpreter/OpcodeType.hpp"
#include "Lox/Interpreter/Opcode.hpp"

namespace Lox
{
    class ChunkDumper
    {
    public:
        ChunkDumper(const Chunk& chunk, std::string_view name, std::ostream& out);
            
        void Disassemble();
        Chunk::CodeIterator DisassembleInstruction(Chunk::CodeIterator location);

        void PrintConstants();

        void Dump();

    private:
        const Chunk& chunk;
        std::string_view name;
        std::ostream& out;

        void PrintCodeTableNames();
        void PrintConstantsTableNames();
        
        #define LOX_OPCODE_TYPE_DUMPER(name) \
            Chunk::CodeIterator name##Instruction(Chunk::CodeIterator location);
        
        LOX_OPCODE_TYPE_LIST(LOX_OPCODE_TYPE_DUMPER);
        
        #undef LOX_OPCODE_TYPE_DUMPER
        
        void PrintConstant(Chunk::ConstantsIterator it);

        void PrintLongLine(char ch, std::size_t n);
    }; // class ChunkDumper
}

#endif // LOX_VM_UTIL_CHUNK_DUMPER_HPP
