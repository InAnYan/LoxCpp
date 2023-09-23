#ifndef LOX_VM_CONFIGURATION_HPP
#define LOX_VM_CONFIGURATION_HPP

#include <cstddef>

namespace Lox::Configuration
{
    constexpr std::size_t FramesCount = 64;
    constexpr std::size_t StackSize = FramesCount * 256;
    
    constexpr bool DebugMode = true;
    constexpr bool DumpChunkAfterCompile = true;
    constexpr bool TraceExecution = true;
    
    constexpr bool StressGC = true;
    constexpr bool LogGC = true;
}

#endif // LOX_VM_CONFIGURATION_HPP
