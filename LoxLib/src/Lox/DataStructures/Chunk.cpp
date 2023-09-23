#include "Lox/DataStructures/Chunk.hpp"

namespace Lox
{
    std::size_t Chunk::PushCode(uint8_t byte, std::size_t line)
    {
        code.push_back(byte);
        codeLines.PushLine(line);
        return code.size() - 1;
    }

    std::size_t Chunk::PushConstant(Value val, std::size_t line)
    {
        /*
        for (ConstantsIterator it = ConstantsBegin(); it != ConstantsEnd(); ++it)
        {
            if (*it == val)
            {
                return it - ConstantsBegin();
            }
        }
         */
        // When we store a constant we also store its position in the source code.
        // If we reuse constant, then the position for new but equal constant
        // will be wrong.

        constants.push_back(val);
        constantsLines.PushLine(line);
        return constants.size() - 1;
    }

    uint8_t Chunk::GetCode(std::size_t index) const
    {
        return code[index];
    }
    
    uint8_t& Chunk::GetCode(std::size_t index)
    {
        return code[index];
    }

    Value Chunk::GetConstant(std::size_t index) const
    {
        return constants[index];
    }

    std::size_t Chunk::GetCodeLine(CodeIterator at) const
    {
        return codeLines[at - code.begin()];
    }

    std::size_t Chunk::GetCodeLine(std::size_t index) const
    {
        return codeLines[index];
    }

    std::size_t Chunk::GetConstantLine(ConstantsIterator at) const
    {
        return constantsLines[at - constants.begin()];
    }

    std::size_t Chunk::GetConstantLine(std::size_t index) const
    {
        return constantsLines[index];
    }
        
    Chunk::CodeIterator Chunk::CodeBegin() const
    {
        return code.cbegin();
    }

    Chunk::CodeIterator Chunk::CodeEnd() const
    {
        return code.cend();
    }

    Chunk::ConstantsIterator Chunk::ConstantsBegin() const
    {
        return constants.cbegin();
    }

    Chunk::ConstantsIterator Chunk::ConstantsEnd() const
    {
        return constants.cend();
    }

    std::size_t Chunk::GetCodeSize() const
    {
        return code.size();
    }

    std::size_t Chunk::GetConstantsCount() const
    {
        return constants.size();
    }
    
    bool Chunk::HasConstant(std::size_t index) const
    {
        return index < constants.size();
    }

    void Chunk::EraseCode(CodeIterator begin, CodeIterator end)
    {
        code.erase(begin, end);
    }

    void Chunk::EraseCode(CodeIterator at)
    {
        code.erase(at);
        codeLines.Erase(at - code.begin());
    }

    void Chunk::EraseCode(std::size_t index)
    {
        code.erase(code.begin() + index);
        codeLines.Erase(index);
    }

    void Chunk::InsertCode(CodeIterator at, uint8_t byte, std::size_t line)
    {
        code.insert(at, byte);
        codeLines.Insert(at - code.begin(), line);
    }

    void Chunk::InsertCode(std::size_t index, uint8_t byte, std::size_t line)
    {
        code.insert(code.begin() + index, byte);
        codeLines.Insert(index, line);
    }
}
