#ifndef LOX_VM_INTERPRETER_VIRTUAL_MACHINE_HPP
#define LOX_VM_INTERPRETER_VIRTUAL_MACHINE_HPP

#include <array>
#include <unordered_map>

#include "Lox/Configuration.hpp"

#include "VirtualMachineConfiguration.hpp"
#include "CallFrame.hpp"

#include "Lox/Runtime/Value.hpp"
#include "Lox/Runtime/MemoryManager.hpp"

#include "Lox/Runtime/Objects/String.hpp"
#include "Lox/Runtime/Objects/Native.hpp"
#include "Lox/Runtime/Objects/Closure.hpp"
#include "Lox/Runtime/Objects/Class.hpp"
#include "Lox/Runtime/Objects/BoundMethod.hpp"

#include "Lox/Compiler/SourcePosition.hpp"

#include "Lox/DataStructures/Chunk.hpp"

#include "Exceptions/RuntimeException.hpp"

namespace Lox
{
    class VirtualMachine : public RootsSource
    {
    public:
        explicit VirtualMachine(const VirtualMachineConfiguration& conf);
        
        void RunScript(GcPtr<Closure> func);
        
        /// Do not allocate string this way. Use InternString.
        template <typename T, typename... Args>
        GcPtr<T> AllocateObject(Args&& ... args)
        {
            return memory.AllocateObject<T>(std::forward<Args&&>(args)...);
        }
        
        GcPtr<String> InternString(std::string_view str);
        
        GcPtr<String> InternStringTake(std::string&& str);
        
        void DefineNative(std::string_view name, std::size_t arity, NativeFn&& fn);
        
        // This method is public in order to allow natives to throw a runtime error.
        template <typename T, typename... Args>
        void ThrowRuntimeException(Args&& ... args)
        {
            SourcePosition pos = CallFramePeek().MakeExceptionSourcePosition();
            
            std::vector<Exceptions::StackTraceEntry> stackTrace = MakeStackTrace();
            
            // NOTE: When we clear stacks by setting its references to top to zero, we
            // should check that the `CallFrame` and `Value` don't have a complex destructor.
            framesCount = 0;
            stackTop = stack.begin();
            throw T(pos, std::move(stackTrace), std::forward<Args&&>(args)...);
        }
    
    private:
        VirtualMachineConfiguration conf;
        
        // Memory stuff.
        
        MemoryManager memory;
        
        void MarkRoots() override;
        
        std::unordered_map<std::string_view, GcPtr<String>> strings;
        
        GcPtr<String> initStr;
        
        // Core.
        
        bool running;
        
        void Run();
        
        std::unordered_map<GcPtr<String>, Value> globals;
        
        GcPtr<Upvalue> openUpvalues;
        
        // Opcodes.
        
        using OpcodeExecFn = void (VirtualMachine::*)();
        std::array<OpcodeExecFn, UINT8_MAX> opcodesExecTable;
        
        void ExecuteOpcode(uint8_t opcode);
        
        void ExecuteUnknownOpcode();
        
        #define LOX_OPCODE_EXEC_FUNC(name, type, _) \
            void Execute##name();
        
        LOX_OPCODES_LIST(LOX_OPCODE_EXEC_FUNC);
        
        #undef LOX_OPCODE_EXEC_FUNC
        
        void LiteralPush(Value val);
        
        GcPtr<String> ReadStringArgument();
        
        // Stack.
        
        using FrameArray = std::array<CallFrame, Configuration::FramesCount>;
        FrameArray frames;
        std::size_t framesCount;
        
        void CallFramePush(CallFrame&& frame);
        
        void CallFramePop();
        
        CallFrame& CallFramePeek(std::size_t offset = 0);
        
        const CallFrame& CallFrameTop() const;
        
        using StackArray = std::array<Value, Configuration::StackSize>;
        StackArray stack;
        StackArray::iterator stackTop;
        
        void StackPush(Value val);
        
        Value StackPop();
        
        Value StackPeek(std::size_t offset = 0); // Throws StackUnderflow if the stack is empty.
        StackIterator StackPeekIterator(std::size_t offset = 0);
        
        Value StackGet(std::size_t index);
        
        void StackSet(std::size_t index, Value newVal);
        
        // Runtime operations.
        
        void CallValue(Value val, uint8_t argCount);
        
        void CallFunction(GcPtr<Closure> func, uint8_t argCount);
        
        void CallNative(GcPtr<Native> func, uint8_t argCount);
        
        void CallClass(GcPtr<Class> klass, uint8_t argCount);
        
        void CallBoundMethod(GcPtr<BoundMethod> method, uint8_t argCount);
        
        void Invoke(GcPtr<Instance> obj, GcPtr<String> name, uint8_t argCount);
        
        void InvokeFromClass(GcPtr<Class> obj, GcPtr<String> name, uint8_t argCount);
        
        GcPtr<BoundMethod> BindMethod(GcPtr<Class> klass, GcPtr<String> name, GcPtr<Instance> obj);
        
        GcPtr<Upvalue> AddLocalUpvalue(StackIterator it);
        
        void CloseUpvalues(StackIterator it);
        
        template <typename T>
        T ExtractValue(Value val);
        
        template <typename T>
        GcPtr<T> ExtractObject(Value val);
        
        template <typename BinOp>
        void BinaryOperation();
        
        GcPtr<String> Concatenate(GcPtr<String> str1, GcPtr<String> str2);
        
        // Debug.
        
        void TraceExecution(std::ostream& out) const;
        
        void PrintStack(std::ostream& out) const;
        
        std::vector<Exceptions::StackTraceEntry> MakeStackTrace() const;
    }; // class VirtualMachine
}

#endif // LOX_VM_INTERPRETER_VIRTUAL_MACHINE_HPP
