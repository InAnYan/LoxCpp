#ifndef LOX_VM_UTIL_CHUNK_WRITER_CONSTANTS_HPP
#define LOX_VM_UTIL_CHUNK_WRITER_CONSTANTS_HPP

#include <cstdint>

#include "../Runtime/Value.hpp"

#include "Lox/Util/Assert.hpp"

namespace Lox
{
    namespace ChunkInfo
    {
        constexpr std::size_t SizeOfSize = 8;

        constexpr std::size_t LineSize = 8;

        constexpr std::size_t TypeMarkSize = 8;
        constexpr std::size_t ValueSize = 8;
    }
}

#endif // LOX_VM_UTIL_CHUNK_WRITER_CONSTANTS_HPP
