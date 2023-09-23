#include "Lox/DataStructures/LinesArray.hpp"

namespace Lox
{
    std::size_t LinesArray::PushLine(std::size_t line)
    {
        array.push_back(line);
        return array.size() - 1;
    }

    std::size_t LinesArray::operator[](std::size_t index) const
    {
        return array[index];
    }

    void LinesArray::Erase(std::size_t index)
    {
        array.erase(array.begin() + index);
    }

    void LinesArray::Insert(std::size_t index, std::size_t line)
    {
        array.insert(array.begin() + index, line);
    }
}
