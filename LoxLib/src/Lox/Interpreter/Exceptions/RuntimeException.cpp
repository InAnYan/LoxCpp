#include "Lox/Interpreter/Exceptions/RuntimeException.hpp"

namespace Lox
{
    namespace Exceptions
    {
        RuntimeException::RuntimeException(SourcePosition pos, std::vector <StackTraceEntry>&& stackTrace,
                                           const char* msg)
                : pos(pos), stackTrace(stackTrace), msg(msg)
        {
        
        }
        
        const char* RuntimeException::what() const noexcept
        {
            return msg;
        }
        
        const std::vector<StackTraceEntry>& RuntimeException::GetStackTrace() const
        {
            return stackTrace;
        }
        
        SourcePosition RuntimeException::GetPos() const
        {
            return pos;
        }
        
        std::ostream& RuntimeException::PrintAdditional(std::ostream& out) const
        {
            return out;
        }
        
        std::ostream& operator<<(std::ostream& out, const RuntimeException& e)
        {
            out << "Runtime error: " << e.what();
            e.PrintAdditional(out) << std::endl;
            out << "Stack trace: " << std::endl;
            
            for (const auto& entry : e.GetStackTrace())
            {
                GcPtr<const Closure> func = entry.first;
                Chunk::CodeIterator ip = entry.second;
                std::size_t line = func->GetChunk().GetCodeLine(ip);
                out << "  " << func->GetName()->GetCppString() << ":" << line << std::endl;
            }
            
            return out;
        }
    }
}
