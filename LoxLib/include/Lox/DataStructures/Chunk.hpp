#ifndef LOX_VM_DATA_STRUCTURES_CHUNK_HPP
#define LOX_VM_DATA_STRUCTURES_CHUNK_HPP

#include <cstdint>
#include <istream>
#include <string>
#include <string_view>
#include <vector>

#include "LinesArray.hpp"

#include "Lox/Runtime/Value.hpp"

#include "Lox/Runtime/Objects/String.hpp"

namespace Lox
{
    class Chunk
    {
    public:
        using CodeIterator = std::vector<uint8_t>::const_iterator;
        using ConstantsIterator = std::vector<Value>::const_iterator;

        std::size_t PushCode(uint8_t byte, std::size_t line);
        std::size_t PushConstant(Value val, std::size_t line);

        uint8_t GetCode(std::size_t index) const;
        uint8_t& GetCode(std::size_t index);

        Value GetConstant(std::size_t index) const;

        std::size_t GetCodeLine(CodeIterator at) const;
        std::size_t GetCodeLine(std::size_t index) const;

        std::size_t GetConstantLine(ConstantsIterator at) const;
        std::size_t GetConstantLine(std::size_t index) const;

        CodeIterator CodeBegin() const;
        CodeIterator CodeEnd() const;

        ConstantsIterator ConstantsBegin() const;
        ConstantsIterator ConstantsEnd() const;

        std::size_t GetCodeSize() const;
        std::size_t GetConstantsCount() const;
        
        bool HasConstant(std::size_t index) const;

        void EraseCode(CodeIterator begin, CodeIterator end);
        void EraseCode(CodeIterator at);
        void EraseCode(std::size_t index);

        void InsertCode(CodeIterator at, uint8_t byte, std::size_t line);
        void InsertCode(std::size_t index, uint8_t byte, std::size_t line);
        
    private:
        std::vector<uint8_t> code;
        LinesArray codeLines;

        std::vector<Value> constants;
        LinesArray constantsLines;
    }; // class Chunk
}

#endif // LOX_VM_DATA_STRUCTURES_CHUNK_HPP
