#ifndef LOX_VM_STAGES_COMPILER_HPP
#define LOX_VM_STAGES_COMPILER_HPP

#include <string_view>

#include "Lox/Interpreter/VirtualMachine.hpp"

#include "Lox/Runtime/Objects/Closure.hpp"

#include "CompilerErrorReporter.hpp"

namespace Lox
{
    GcPtr<Closure> Compile(VirtualMachine& vm, CompilerErrorReporter& errorReporter, std::string_view filePath,
                           std::string_view input);
}

#endif // LOX_VM_STAGES_COMPILER_HPP
