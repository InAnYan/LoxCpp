#ifndef LOX_VM_STAGES_CHUNK_CHECKER_HPP
#define LOX_VM_STAGES_CHUNK_CHECKER_HPP

#include "Lox/DataStructures/Chunk.hpp"

namespace Lox
{
    class ChunkChecker
    {
    public:
        ChunkChecker(const Chunk& chunk);
            
        bool Check();

    private:
        const Chunk& chunk;
        
        int stackSize;
    }; // class ChunkChecker
}

#endif // LOX_VM_STAGES_CHUNK_CHECKER_HPP
