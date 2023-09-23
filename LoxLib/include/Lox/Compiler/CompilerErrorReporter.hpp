#ifndef LOX_VM_STAGES_COMPILER_ERROR_REPORTER_HPP
#define LOX_VM_STAGES_COMPILER_ERROR_REPORTER_HPP

#include <string_view>

#include "Lox/Compiler/SourcePosition.hpp"

namespace Lox
{
    class CompilerErrorReporter
    {
    public:
        virtual ~CompilerErrorReporter() = default;

        virtual void Error(SourcePosition pos, std::string_view msg) = 0;
    }; // class CompilerErrorReporter
}

#endif // LOX_VM_STAGES_COMPILER_ERROR_REPORTER_HPP
