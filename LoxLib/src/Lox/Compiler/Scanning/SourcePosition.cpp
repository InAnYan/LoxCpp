#include "Lox/Compiler/SourcePosition.hpp"

namespace Lox    
{
    SourcePosition::SourcePosition(std::size_t line)
        : line(line)
    {
            
    }

    std::size_t SourcePosition::GetLine() const
    {
        return line;
    }

    bool SourcePosition::operator==(const SourcePosition& other) const
    {
        return this->line == other.line;
    }

    std::ostream& operator<<(std::ostream& out, const SourcePosition& pos)
    {
        return out << pos.GetLine();
    }
}
