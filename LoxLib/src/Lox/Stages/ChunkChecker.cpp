#include "Lox/Stages/ChunkChecker.hpp"

#include "Lox/Configuration.hpp"

#include "Lox/Interpreter/Opcode.hpp"

namespace Lox
{
    ChunkChecker::ChunkChecker(const Chunk& chunk)
            : chunk(chunk), stackSize(0)
    {
    
    }
    
    bool ChunkChecker::Check()
    {
        // TODO: Is that even the right check?
        // TODO: How to check with jumps?
        
        return true;
        
        auto ip = chunk.CodeBegin();
        
        while (true)
        {
            if (ip == chunk.CodeEnd())
            {
                return false;
            }
            
            uint8_t byte = *ip++;
            auto opcode = static_cast<Opcode>(byte);
            
            switch (opcode)
            {
            case OpcodeNot:
            case OpcodeNegate:
            case OpcodeEqual:
            case OpcodePrint:
                if (stackSize < 1)
                {
                    return false;
                }
                break;
            
            case OpcodeAdd:
            case OpcodeSubstract:
            case OpcodeMultiply:
            case OpcodeDivide:
            case OpcodeLess:
            case OpcodeGreater:
                if (stackSize < 2)
                {
                    return false;
                }
                break;
            
            case OpcodePop:
                if (stackSize == 0)
                {
                    return false;
                }
                break;
            
            case OpcodeNil:
            case OpcodeTrue:
            case OpcodeFalse:
                break;
            
            case OpcodeJump:
            case OpcodeJumpIfFalse:
            {
                if (ip == chunk.CodeEnd())
                {
                    return false;
                }
                
                uint8_t first = *ip++;
                
                if (ip == chunk.CodeEnd())
                {
                    return false;
                }
                
                uint8_t second = *ip++;
                
                uint16_t addr = (first << 8) | second;
                
                break;
            }
            
            case OpcodeDefineGlobal:
            case OpcodeGetGlobal:
            case OpcodeSetGlobal:
            case OpcodePushConstant:
            {
                if (ip == chunk.CodeEnd())
                {
                    return false;
                }
                
                uint8_t index = *ip++;
                
                if (!chunk.HasConstant(index)
                    || (opcode != OpcodePushConstant && !chunk.GetConstant(index).IsObject(ObjectType::String)))
                {
                    return false;
                }
                
                if (opcode == OpcodeSetGlobal)
                {
                    if (stackSize < 1)
                    {
                        return false;
                    }
                }
                
                break;
            }
            
            case OpcodeSetLocal:
            case OpcodeGetLocal:
            {
                if (opcode == OpcodeSetLocal && stackSize < 1)
                {
                    return false;
                }
                
                if (ip == chunk.CodeEnd())
                {
                    return false;
                }
                
                uint8_t index = *ip++;
                
                if (index >= stackSize)
                {
                    return false;
                }
                
                break;
            }
            
            case OpcodeReturn:
            {
                if (ip == chunk.CodeEnd())
                {
                    return stackSize == 0;
                }
                
                break;
            }
            
            default:
                return false;
            }
            
            stackSize += GetOpcodeStackEffect(opcode);
            
            if (stackSize > Configuration::StackSize || stackSize < 0)
            {
                return false;
            }
        }
    }
}
