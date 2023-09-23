#include "Lox/Util/ChunkDumper.hpp"

#include <cstddef>
#include <cstring>
#include <iomanip>

#include "Lox/Interpreter/Opcode.hpp"
#include "Lox/Interpreter/OpcodeType.hpp"
#include "Lox/Util/Assert.hpp"

namespace Lox
{
    template <typename Fn>
    void Repeat(std::size_t n, Fn fn)
    {
        while (n--)
        {
            fn();
        }
    }
    
    ChunkDumper::ChunkDumper(const Chunk& chunk, std::string_view name, std::ostream& out)
            : chunk(chunk), name(name), out(out)
    {
    
    }
    
    void ChunkDumper::Disassemble()
    {
        PrintCodeTableNames();
        // PrintLongLine('-', 45);
        
        for (auto it = chunk.CodeBegin();
             it != chunk.CodeEnd();
             it = DisassembleInstruction(it))
        {
        
        }
    }
    
    void ChunkDumper::PrintConstants()
    {
        PrintConstantsTableNames();
        // PrintLongLine('-', 45);
        
        for (auto it = chunk.ConstantsBegin();
             it != chunk.ConstantsEnd();
             ++it)
        {
            PrintConstant(it);
        }
    }
    
    void ChunkDumper::Dump()
    {
        PrintLongLine('=', 45);
        out << "Chunk dump: " << name << std::endl;
        PrintLongLine('-', 45);
        out << "Code:" << std::endl;
        Disassemble();
        out << std::endl;
        out << "Constants:" << std::endl;
        PrintConstants();
        PrintLongLine('=', 45);
    }
    
    void ChunkDumper::PrintConstant(Chunk::ConstantsIterator it)
    {
        std::size_t constantIndex = it - chunk.ConstantsBegin();
        out << std::setw(4) << std::setfill('0') << constantIndex << ' '
            << std::setw(4) << chunk.GetConstantLine(constantIndex) << ' '
            << std::left << std::setw(8) << std::setfill(' ');
        
        if (it->IsObject())
        {
            out << it->AsObject()->GetType();
        }
        else
        {
            out << it->GetType();
        }
        
        out << ' ' << std::right
            << *it << std::endl;
    }
    
    void ChunkDumper::PrintCodeTableNames()
    {
        out << "OFFS LINE";
        out << " " << std::left << std::setw(16) << "OPCODE" << std::right << " ";
        out << "VALUE/ADDR" << std::endl;
    }
    
    void ChunkDumper::PrintConstantsTableNames()
    {
        out << "OFFS LINE TYPE     DATA" << std::endl;
    }
    
    Chunk::CodeIterator ChunkDumper::DisassembleInstruction(Chunk::CodeIterator location)
    {
        std::ptrdiff_t offset = location - chunk.CodeBegin();
        auto opcode = static_cast<Opcode>(*location);
        out << std::setfill('0') << std::setw(4) << offset << " "
            << std::setw(4) << chunk.GetCodeLine(location - chunk.CodeBegin()) << std::setfill(' ') << ' '
            << std::setw(16) << std::left << opcode << " " << std::right;
        
        switch (GetOpcodeType(opcode))
        {
            #define LOX_OPCODE_TYPE_DUMP_SWITCH(name)      \
                case OpcodeType::name:                   \
                    return name##Instruction(location);
        
        LOX_OPCODE_TYPE_LIST(LOX_OPCODE_TYPE_DUMP_SWITCH);
            
            #undef LOX_OBJECT_TYPE_DUMP
        
        default:
            LOX_UNREACHABLE("Lox::ChunkDumper::DisassembleInstruction unreachable by GetOpcodeType");
            return location; // To suppress warning.
        }
    }
    
    Chunk::CodeIterator ChunkDumper::SimpleInstruction(Chunk::CodeIterator location)
    {
        out << std::endl;
        return location + 1;
    }
    
    Chunk::CodeIterator ChunkDumper::ConstantInstruction(Chunk::CodeIterator location)
    {
        if (location == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 1;
        }
        
        auto constantIndexLocation = static_cast<uint8_t>(location[1]);
        
        if (chunk.HasConstant(constantIndexLocation))
        {
            out << chunk.GetConstant(constantIndexLocation);
        }
        else
        {
            out << "## wrong constant ##";
        }
        
        out << std::endl;
        
        return location + 2;
    }
    
    Chunk::CodeIterator ChunkDumper::ByteInstruction(Chunk::CodeIterator location)
    {
        if (location + 1 == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 1;
        }
        
        uint8_t index = location[1];
        
        out << std::setw(4) << std::left << static_cast<int>(index) << std::right << std::endl;
        
        return location + 2;
    }
    
    Chunk::CodeIterator ChunkDumper::JumpInstruction(Chunk::CodeIterator location)
    {
        if (location + 1 == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 1;
        }
        
        uint8_t first = location[1];
        
        if (location + 2 == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 2;
        }
        
        uint8_t second = location[2];
        
        uint16_t addr = (first << 8) | second;
        
        out << std::setfill('0') << std::setw(4) << (location - chunk.CodeBegin()) + addr + 3
            << std::setfill(' ') << std::endl;
        
        return location + 3;
    }
    
    Chunk::CodeIterator ChunkDumper::LoopInstruction(Chunk::CodeIterator location)
    {
        if (location + 1 == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 1;
        }
        
        uint8_t first = location[1];
        
        if (location + 2 == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 2;
        }
        
        uint8_t second = location[2];
        
        uint16_t addr = (first << 8) | second;
        
        out << std::setfill('0') << std::setw(4) << (location - chunk.CodeBegin()) - addr + 3
            << std::setfill(' ') << std::endl;
        
        return location + 3;
    }
    
    Chunk::CodeIterator ChunkDumper::ClosureInstruction(Chunk::CodeIterator location)
    {
        ++location;
        
        if (location == chunk.CodeEnd())
        {
            out << "## no arguments ##" << std::endl;
            return location;
        }
        
        uint8_t count = *location++;
        out << static_cast<int>(count) << std::endl;
        
        for (int i = 0; i < count; ++i)
        {
            if (location == chunk.CodeEnd())
            {
                out << "## not enough arguments ##" << std::endl;
                break;
            }
            
            uint8_t isLocal = *location++;
            
            if (location == chunk.CodeEnd())
            {
                out << "## not enough arguments ##" << std::endl;
                break;
            }
            
            uint8_t index = *location++;
            
            if (isLocal)
            {
                out << "local ";
            }
            else
            {
                out << "upvalue ";
            }
            
            out << static_cast<int>(index) << std::endl;
        }
        
        return location;
    }
    
    Chunk::CodeIterator ChunkDumper::InvokeInstruction(Chunk::CodeIterator location)
    {
        if (location == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 1;
        }
        
        // Well, I could cast to int from the beginning.
        auto argCount = static_cast<uint8_t>(location[1]);
        out << static_cast<int>(argCount);
        
        out << " ";
        
        if (location + 1 == chunk.CodeEnd())
        {
            out << "## no argument ##" << std::endl;
            return location + 2;
        }
        
        auto constantIndexLocation = static_cast<uint8_t>(location[2]);
        
        if (chunk.HasConstant(constantIndexLocation))
        {
            out << chunk.GetConstant(constantIndexLocation);
        }
        else
        {
            out << "## wrong constant ##";
        }
        
        out << std::endl;
        
        return location + 3;
    }
    
    void ChunkDumper::PrintLongLine(char ch, std::size_t n)
    {
        for (int i = 0; i < n; i++)
        {
            // I could create a string using its copy character constructor.
            // But I don't want to allocate memory. (Well, probably compiler could optimise this).
            out << ch;
        }
        
        out << std::endl;
    }
}
