#include "Lox/Compiler/Compiler.hpp"

#include "Compilers/Compiler.hpp"

namespace Lox
{
    GcPtr<Closure> Compile(VirtualMachine& vm, CompilerErrorReporter& errorReporter, std::string_view filePath,
                           std::string_view input)
    {
        Parser parser(errorReporter, filePath, input);
        Compiler compiler(vm, std::nullopt, std::nullopt, parser, FunctionType::Script,
                          Token(TokenType::Identifier, SourcePosition(1), "<script>"));
        return compiler.Compile();
    }
}