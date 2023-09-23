#ifndef LOX_VM_INTERPRETER_OPCODE_TYPE_HPP
#define LOX_VM_INTERPRETER_OPCODE_TYPE_HPP

#define LOX_OPCODE_TYPE_LIST(o)  \
    o(Simple)                    \
    o(Constant)                  \
    o(Byte)                      \
    o(Jump)                      \
    o(Loop)                      \
    o(Closure)                   \
    o(Invoke)

namespace Lox
{
    enum class OpcodeType
    {
        #define LOX_OPCODE_TYPE_ENUM(name) \
            name,
        
        LOX_OPCODE_TYPE_LIST(LOX_OPCODE_TYPE_ENUM)
        
        #undef LOX_OPCODE_TYPE_ENUM
    }; // enum class OpcodeType
}

#endif // LOX_VM_INTERPRETER_OPCODE_TYPE_HPP
