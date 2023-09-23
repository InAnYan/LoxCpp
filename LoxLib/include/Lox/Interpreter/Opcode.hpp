#ifndef LOX_VM_INTERPRETER_OPCODE_HPP
#define LOX_VM_INTERPRETER_OPCODE_HPP

#include <ostream>
#include <cstdint>

#include "OpcodeType.hpp"

#include "Lox/Runtime/Value.hpp"

#define LOX_OPCODES_LIST(o)                         \
    o(PushConstant, Constant, +1)                   \
    o(Print, Simple, -1)                            \
    o(Add, Simple, -1)                              \
    o(Substract, Simple, -1)                        \
    o(Multiply, Simple, -1)                         \
    o(Divide, Simple, -1)                           \
    o(Return, Simple, 0)                            \
    o(Nil, Simple, 1)                               \
    o(True, Simple, 1)                              \
    o(False, Simple, 1)                             \
    o(Greater, Simple, -1)                          \
    o(Less, Simple, -1)                             \
    o(Not, Simple, 0)                               \
    o(Negate, Simple, 0)                            \
    o(Equal, Simple, -1)                            \
    o(Pop, Simple, -1)                              \
    o(DefineGlobal, Constant, -1)                   \
    o(GetGlobal, Constant, 1)                       \
    o(SetGlobal, Constant, 0)                       \
    o(GetLocal, Byte, +1)                           \
    o(SetLocal, Byte, 0)                            \
    o(Jump, Jump, 0)                                \
    o(JumpIfFalse, Jump, 0)                         \
    o(Loop, Loop, 0)                                \
    o(Call, Byte, 0)                                \
    o(GetUpvalue, Byte, 1)                          \
    o(SetUpvalue, Byte, 0)                          \
    o(FillUpvalues, Closure, 0)                     \
    o(CloseUpvalue, Simple, -1)                     \
    o(Class, Constant, 0)                           \
    o(GetProperty, Constant, 1)                     \
    o(SetProperty, Constant, -1)                    \
    o(Method, Constant, -1)                         \
    o(Invoke, Invoke, 0)                            \
    o(Inherit, Simple, -1)                          \
    o(GetSuper, Constant, -1)                       \
    o(InvokeSuper, Invoke, 0)

// Well, the stack effect of OpcodeCall is not that simple.

namespace Lox
{
    enum Opcode : uint8_t
    {
        #define LOX_OPCODES_ENUM(name, _, __) Opcode##name,
        
        LOX_OPCODES_LIST(LOX_OPCODES_ENUM)
        
        #undef LOX_OPCODES_ENUM
    }; // enum class Opcode
    
    std::ostream& operator<<(std::ostream& out, Opcode opcode);
    
    OpcodeType GetOpcodeType(Opcode opcode);
    
    constexpr unsigned MAX_PUSH_CONSTANT = 256;
    
    bool IsLiteralOpcode(Opcode opcode);
    
    Value LiteralOpcodeToValue(Opcode opcode);
    
    int GetOpcodeStackEffect(Opcode opcode);
}

#endif // LOX_VM_INTERPRETER_OPCODE_HPP
