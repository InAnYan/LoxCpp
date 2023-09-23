#ifndef LOX_VM_DATA_STRUCTURES_LINES_ARRAY_HPP
#define LOX_VM_DATA_STRUCTURES_LINES_ARRAY_HPP

#include <cstdint>
#include <vector>

namespace Lox
{
    class LinesArray
    {
    public:
        LinesArray() = default;

        std::size_t PushLine(std::size_t line);

        std::size_t operator[](std::size_t index) const;

        void Erase(std::size_t index);

        void Insert(std::size_t index, std::size_t line);
        void Insert(std::size_t begin, std::size_t end, std::size_t line);

    private:
        std::vector<std::size_t> array;
    }; // class LinesArra
}

#endif // LOX_VM_DATA_STRUCTURES_LINES_ARRAY_HPP
