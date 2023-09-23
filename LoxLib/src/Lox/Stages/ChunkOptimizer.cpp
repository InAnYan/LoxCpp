#include "Lox/Stages/ChunkOptimizer.hpp"

#include <type_traits>

#include "Lox/Configuration.hpp"

#include "Lox/Interpreter/Opcode.hpp"

#include "Lox/Interpreter/Exceptions/ZeroDivision.hpp"
#include "Lox/Interpreter/Exceptions/StackOverflow.hpp"
#include "Lox/Interpreter/Exceptions/StackUnderflow.hpp"
#include "Lox/Interpreter/Exceptions/WrongType.hpp"

#include "Lox/Runtime/Value.hpp"

#include "Lox/Compiler/SourcePosition.hpp"

#include "Lox/Util/Assert.hpp"

#include "Lox/Stages/Exceptions/OptimizerFailure.hpp"

namespace Lox
{
    ChunkOptimizer::ChunkOptimizer(VirtualMachine& vm, Chunk& chunk)
            : vm(vm), chunk(chunk), ip(0)
    {
    
    }
    
    void ChunkOptimizer::Optimize()
    {
        for (; ip < chunk.GetCodeSize(); ++ip)
        {
            switch (static_cast<Opcode>(chunk.GetCode(ip)))
            {
            case OpcodePushConstant:
            {
                StackPush(ip);
                ++ip; // Skip argument.
                break;
            }
            
            case OpcodeNot:
            {
                if (constantsStack.empty())
                {
                    break;
                }
                
                std::size_t line = chunk.GetCodeLine(ip);
                
                Value oldVal = PopValue(StackPop());
                Value newVal(oldVal.IsFalse());
                RemoveInstruction(ip);
                PushValue(newVal, line, ip);
                --ip; // Reread value.
                break;
            }
            
            case OpcodeNegate:
            {
                if (constantsStack.empty())
                {
                    break;
                }
                
                std::size_t line = chunk.GetCodeLine(ip);
                
                Value oldVal = PopValue(StackPop());
                Value newVal = NegateValue(oldVal);
                RemoveInstruction(ip);
                PushValue(newVal, line, ip);
                --ip; // Reread value.
                break;
            }
            
            case OpcodeNil:
            case OpcodeTrue:
            case OpcodeFalse:
            {
                StackPush(ip);
                break;
            }
            
            case OpcodeAdd:
            {
                BinaryOperation<std::plus<double>>();
                break;
            }
            
            case OpcodeSubstract:
            {
                BinaryOperation<std::minus<double>>();
                break;
            }
            
            case OpcodeMultiply:
            {
                BinaryOperation<std::multiplies<double>>();
                break;
            }
            
            case OpcodeDivide:
            {
                BinaryOperation<std::divides<double>>();
                break;
            }
            
            case OpcodeGreater:
            {
                BinaryOperation<std::greater<double>>();
                break;
            }
            
            case OpcodeLess:
            {
                BinaryOperation<std::less<double>>();
                break;
            }
            
            case OpcodeEqual:
            {
                if (constantsStack.size() < 2)
                {
                    constantsStack.clear();
                    break;
                }
                
                std::size_t line = chunk.GetCodeLine(ip);
                
                Value b = PopValue(StackPop());
                Value a = PopValue(StackPop());
                
                Value newVal(a == b);
                
                RemoveInstruction(ip);
                PushValue(newVal, line, ip);
                --ip; // Reread the value.
                
                break;
            }
            
            default:
                constantsStack.clear();
                break;
            }
        }
    }
    
    GcPtr<String> ChunkOptimizer::ReadString()
    {
        uint8_t stringIndex = chunk.GetCode(++ip);
        return chunk.GetConstant(stringIndex).AsObject<String>();
    }
    
    template <typename BinOp>
    void ChunkOptimizer::BinaryOperation()
    {
        if (constantsStack.size() < 2)
        {
            constantsStack.clear();
            return;
        }
        
        std::size_t line = chunk.GetCodeLine(ip);
        
        Value b = PopValue(StackPop());
        Value a = PopValue(StackPop());
        Value newVal = PerformBinaryOperation<BinOp>(a, b);
        
        RemoveInstruction(ip);
        PushValue(newVal, line, ip);
        --ip; // Reread the opcode.
    }
    
    template <typename Op>
    Value ChunkOptimizer::PerformBinaryOperation(Value a, Value b)
    {
        if (std::is_same_v<Op, std::plus<double>> && a.IsObject<String>() && b.IsObject<String>())
        {
            GcPtr<String> str1 = a.AsObject<String>();
            GcPtr<String> str2 = b.AsObject<String>();
            return Value(Concatenate(str1, str2));
        }
        
        if (!a.IsDouble())
        {
            ThrowRuntimeException<Exceptions::WrongTypeValue>(a, ValueType::Double);
        }
        
        if (!b.IsDouble())
        {
            ThrowRuntimeException<Exceptions::WrongTypeValue>(b, ValueType::Double);
        }
        
        if (std::is_same_v<Op, std::divides<double>>)
        {
            if (b.AsDouble() == 0)
            {
                ThrowRuntimeException<Exceptions::ZeroDivision>();
            }
        }
        
        return Value(Op{}(a.AsDouble(), b.AsDouble()));
    }
    
    Value ChunkOptimizer::NegateValue(Value val)
    {
        if (!val.IsDouble())
        {
            ThrowRuntimeException<Exceptions::WrongTypeValue>(val, ValueType::Double);
        }
        
        return Value(-val.AsDouble());
    }
    
    Value ChunkOptimizer::PeekValue(std::size_t index)
    {
        auto opcode = static_cast<Opcode>(chunk.GetCode(index));
        
        switch (opcode)
        {
        case OpcodeNil:
            return NIL;
        case OpcodeTrue:
            return Value(true);
        case OpcodeFalse:
            return Value(false);
        case OpcodePushConstant:
        {
            uint8_t constantIndex = chunk.GetCode(index + 1);
            return chunk.GetConstant(constantIndex);
        }
        default:
            LOX_UNREACHABLE("wrong peek value");
            return NIL;
        }
    }
    
    Value ChunkOptimizer::PopValue(std::size_t index)
    {
        // This function changes the ip.
        Value val = PeekValue(index);
        RemoveInstruction(index);
        return val;
    }
    
    void ChunkOptimizer::PushValue(Lox::Value val, std::size_t line, std::size_t index)
    {
        // This function won't change the ip.
        
        switch (val.GetType())
        {
        case ValueType::Nil:
            chunk.InsertCode(index, OpcodeNil, line);
            break;
        case ValueType::Bool:
            chunk.InsertCode(index, val.AsBool() ? OpcodeTrue : OpcodeFalse, line);
            break;
        case ValueType::Object:
        case ValueType::Double:
        {
            std::size_t constantIndex = chunk.PushConstant(val, line);
            
            if (constantIndex > MAX_PUSH_CONSTANT)
            {
                throw Exceptions::OptimizerFailure("too many constants");
            }
            
            chunk.InsertCode(index, OpcodePushConstant, line);
            chunk.InsertCode(index + 1, constantIndex, line);
            
            break;
        }
        }
    }
    
    template <typename T, typename... Args>
    void ChunkOptimizer::ThrowRuntimeException(Args&& ... args)
    {
        // TODO: GcPtr<String> and string view. Will it be saved?
        SourcePosition pos(chunk.GetCodeLine(ip));
        throw T(pos, {}, std::forward<Args&&>(args)...);
    }
    
    GcPtr<String> ChunkOptimizer::Concatenate(GcPtr<String> a, GcPtr<String> b)
    {
        const std::string& str1 = a->GetCppString();
        const std::string& str2 = b->GetCppString();
        
        std::string newStr = str1 + str2;
        
        return vm.InternStringTake(std::move(newStr));
    }
    
    void ChunkOptimizer::StackPush(std::size_t element)
    {
        if (constantsStack.size() == Configuration::StackSize)
        {
            ThrowRuntimeException<Exceptions::StackOverflow>();
        }
        
        constantsStack.push_back(element);
    }
    
    std::size_t ChunkOptimizer::StackPop()
    {
        if (constantsStack.empty())
        {
            ThrowRuntimeException<Exceptions::StackUnderflow>();
        }
        
        std::size_t index = constantsStack.back();
        constantsStack.pop_back();
        return index;
    }
    
    std::size_t ChunkOptimizer::StackPeek()
    {
        if (constantsStack.empty())
        {
            ThrowRuntimeException<Exceptions::StackUnderflow>();
        }
        
        return constantsStack.back();
    }
    
    void ChunkOptimizer::RemoveInstruction(std::size_t at)
    {
        auto opcode = static_cast<Opcode>(chunk.GetCode(at));
        
        unsigned removeCount = 1;
        
        switch (GetOpcodeType(opcode))
        {
        case OpcodeType::Constant:
        case OpcodeType::Byte:
            removeCount++;
            break;
        default:
            break;
        }
        
        for (unsigned i = 0; i < removeCount; i++)
        {
            chunk.EraseCode(at);
            if (ip != 0 && at < ip)
            {
                --ip;
            }
        }
    }
}
