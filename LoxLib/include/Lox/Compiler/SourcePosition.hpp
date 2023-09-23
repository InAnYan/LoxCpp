#ifndef LOX_VM_COMPILER_SOURCE_POSITION_HPP
#define LOX_VM_COMPILER_SOURCE_POSITION_HPP

#include <cstdint>
#include <ostream>
#include <string_view>

namespace Lox
{
    class SourcePosition
    {
    public:
        SourcePosition(std::size_t line);
        
        std::size_t GetLine() const;

        bool operator==(const SourcePosition& other) const;

    private:
        std::size_t line;
    }; // class SourcePosition

    std::ostream& operator<<(std::ostream& out, const SourcePosition& pos);
}

#endif // LOX_VM_COMPILER_SOURCE_POSITION_HPP
