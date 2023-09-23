#ifndef LOX_VM_STAGES_OPTIMIZER_HPP
#define LOX_VM_STAGES_OPTIMIZER_HPP

#include "Lox/Configuration.hpp"

#include "Lox/DataStructures/Chunk.hpp"

#include "Lox/Runtime/Value.hpp"

#include "Lox/Runtime/Objects/String.hpp"

#include "Lox/Compiler/SourcePosition.hpp"

#include "Lox/Interpreter/Opcode.hpp"
#include "Lox/Interpreter/VirtualMachine.hpp"

namespace Lox
{
    // WARNING: About bad design decisions.
    // Optimizer changes the chunk, and it can add a new constant.
    // There can be too many constants.
    // And optimizer cannot recover from it.
    // And LoxInterpreter doesn't save the original chunk.
    
    // Oh, and also Optimizer duplicates VM's binary operation methods.
    
    /// Performs constant folding.
    /// It won't delete unused constants.
    class ChunkOptimizer
    {
    public:
        ChunkOptimizer(VirtualMachine& vm, Chunk& chunk);

        void Optimize();

    private:
        VirtualMachine& vm;
        Chunk& chunk;
        
        std::size_t ip;
        
        std::vector<std::size_t> constantsStack;
        
        void StackPush(std::size_t element);
        std::size_t StackPop();
        std::size_t StackPeek();

        Value PopValue(std::size_t index);
        void PushValue(Value val, std::size_t line, std::size_t index);
        Value PeekValue(std::size_t index);
        
        void RemoveInstruction(std::size_t at);
        
        GcPtr<String> ReadString();
        
        template <typename Op>
        void BinaryOperation();
        
        Value NegateValue(Value val);

        template <typename Op>
        Value PerformBinaryOperation(Value a, Value b);
        
        GcPtr<String> Concatenate(GcPtr<String> a, GcPtr<String> b);

        template <typename T, typename... Args>
        void ThrowRuntimeException(Args&&... args);
    }; // class Optimizer
}

#endif // LOX_VM_STAGES_OPTIMIZER_HPP
