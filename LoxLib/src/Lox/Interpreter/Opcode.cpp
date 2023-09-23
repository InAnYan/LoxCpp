#include "Lox/Interpreter/Opcode.hpp"

namespace Lox
{
    std::ostream& operator<<(std::ostream& out, Opcode opcode)
    {
        #define LOX_OPCODES_PRINT(name, _, __) case Opcode##name: out << #name; break;
        
        switch (opcode)
        {
            LOX_OPCODES_LIST(LOX_OPCODES_PRINT);
        
        default:
            out << "Unknown";
            break;
        }
        
        return out;
        
        #undef LOX_OPCODES_PRINT
    }
    
    OpcodeType GetOpcodeType(Opcode opcode)
    {
        #define LOX_OPCODES_TYPE_GET(name, type, _) case Opcode##name: return OpcodeType::type;
        
        switch (opcode)
        {
            LOX_OPCODES_LIST(LOX_OPCODES_TYPE_GET);
        
        default:
            return OpcodeType::Simple;
        }
        
        #undef LOX_OPCODES_TYPE_GET
    }
    
    bool IsLiteralOpcode(Opcode opcode)
    {
        switch (opcode)
        {
        case OpcodeNil:
        case OpcodeTrue:
        case OpcodeFalse:
            return true;
        default:
            return false;
        }
    }
    
    Value LiteralOpcodeToValue(Opcode opcode)
    {
        switch (opcode)
        {
        case OpcodeNil:
            return NIL;
        case OpcodeTrue:
            return Value(true);
        case OpcodeFalse:
            return Value(false);
        default:
            LOX_UNREACHABLE("wrong literal opcode");
            return NIL;
        }
    }
    
    int GetOpcodeStackEffect(Opcode opcode)
    {
        #define LOX_OPCODES_STACK_EFFECT(name, _, effect) \
            case Opcode##name: return effect;
        
        switch (opcode)
        {
            LOX_OPCODES_LIST(LOX_OPCODES_STACK_EFFECT);
        default:
            return 0;
        }
        
        #undef LOX_OPCODE_STACK_EFFECT
    }
}
