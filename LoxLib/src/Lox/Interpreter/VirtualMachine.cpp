#include "Lox/Interpreter/VirtualMachine.hpp"

#include <functional>

#include "Lox/Configuration.hpp"

#include "Lox/Interpreter/Exceptions/All.hpp"

#include "Lox/Util/Assert.hpp"
#include "Lox/Util/ChunkDumper.hpp"

#include "Lox/Runtime/Objects/Class.hpp"
#include "Lox/Runtime/Objects/Instance.hpp"
#include "Lox/Runtime/Objects/BoundMethod.hpp"

namespace Lox
{
    // Public.
    
    VirtualMachine::VirtualMachine(const VirtualMachineConfiguration& conf)
            : conf(conf), memory(*this), initStr(InternString("init")),
              running(false), openUpvalues(GcPtr<Upvalue>(nullptr)), framesCount(0),
              stackTop(stack.begin()), opcodesExecTable{}
    {
        std::fill(opcodesExecTable.begin(),
                  opcodesExecTable.end(),
                  &VirtualMachine::ExecuteUnknownOpcode);
        
        #define LOX_OPCODES_FILL_TABLE(name, type, _)   \
            opcodesExecTable[Opcode##name] = &VirtualMachine::Execute##name;
        
        LOX_OPCODES_LIST(LOX_OPCODES_FILL_TABLE);
        
        #undef LOX_OPCODES_FILL_TABLE
    }
    
    void VirtualMachine::RunScript(GcPtr<Closure> func)
    {
        StackPush(Value(func));
        CallFramePush(CallFrame(func, StackPeekIterator()));
        
        memory.AllowGC();
        
        running = true;
        Run();
        
        memory.DisallowGC();
    }
    
    GcPtr<String> VirtualMachine::InternString(std::string_view str)
    {
        auto it = strings.find(str);
        
        if (it != strings.end())
        {
            return it->second;
        }
        
        auto obj = AllocateObject<String>(std::string(str));
        strings[obj->GetCppString()] = obj;
        return obj;
    }
    
    GcPtr<String> VirtualMachine::InternStringTake(std::string&& str)
    {
        auto it = strings.find(str);
        
        if (it != strings.end())
        {
            return it->second;
        }
        
        auto obj = AllocateObject<String>(std::move(str));
        strings[obj->GetCppString()] = obj;
        return obj;
    }
    
    void VirtualMachine::DefineNative(std::string_view name, std::size_t arity, NativeFn&& fn)
    {
        GcPtr<String> str = InternString(name);
        globals.emplace(str, AllocateObject<Native>(str, arity, fn));
    }
    
    // Core.
    
    void VirtualMachine::Run()
    {
        while (running)
        {
            CallFrame& frame = CallFramePeek();
            
            if constexpr (Configuration::TraceExecution)
            {
                TraceExecution(conf.GetDebugOutput());
            }
            
            if (frame.IsAtEnd())
            {
                // TODO: Maybe error?
                running = false;
                break;
            }
            
            uint8_t opcode = frame.ReadByte();
            ExecuteOpcode(opcode);
        }
    }
    
    // Opcodes.
    
    void VirtualMachine::ExecuteOpcode(uint8_t opcode)
    {
        OpcodeExecFn fn = opcodesExecTable[opcode];
        (this->*fn)();
    }
    
    GcPtr<String> VirtualMachine::ReadStringArgument()
    {
        return ExtractObject<String>(CallFramePeek().ReadConstant());
    }
    
    void VirtualMachine::ExecuteUnknownOpcode()
    {
        uint8_t byte = CallFramePeek().GetPreviousByte();
        ThrowRuntimeException<Exceptions::UnknownInstruction>(byte);
    }
    
    void VirtualMachine::ExecuteReturn()
    {
        Value result = StackPop();
        
        // There are some tricks with `CallFramePop`.
        // The thing is, there could be a problem with the final
        // stack pop call. We need to save the information of the
        // script function. So we pop the final call frame after
        // the stack pop.
        
        if (framesCount == 1)
        {
            CallFramePop();
            // Ignoring the result of the script function.
            running = false;
        }
        else
        {
            CloseUpvalues(CallFrameTop().GetValues());
            CallFramePop();
            StackPush(result);
        }
    }
    
    void VirtualMachine::ExecutePushConstant()
    {
        Value constant = CallFramePeek().ReadConstant();
        StackPush(constant);
    }
    
    void VirtualMachine::ExecutePrint()
    {
        Value val = StackPop();
        val.Print(conf.GetUserOutput(), PrintFlags::Pretty) << std::endl;
    }
    
    void VirtualMachine::ExecuteNil()
    {
        LiteralPush(NIL);
    }
    
    void VirtualMachine::ExecuteTrue()
    {
        LiteralPush(Value(true));
    }
    
    void VirtualMachine::ExecuteFalse()
    {
        LiteralPush(Value(false));
    }
    
    void VirtualMachine::LiteralPush(Lox::Value val)
    {
        StackPush(val);
    }
    
    void VirtualMachine::ExecuteAdd()
    {
        BinaryOperation<std::plus<DoubleRepr>>();
    }
    
    void VirtualMachine::ExecuteSubstract()
    {
        BinaryOperation<std::minus<DoubleRepr>>();
    }
    
    void VirtualMachine::ExecuteMultiply()
    {
        BinaryOperation<std::multiplies<DoubleRepr>>();
    }
    
    void VirtualMachine::ExecuteDivide()
    {
        BinaryOperation<std::divides<DoubleRepr>>();
    }
    
    void VirtualMachine::ExecuteNot()
    {
        Value val = StackPop();
        StackPush(Value(val.IsFalse()));
    }
    
    void VirtualMachine::ExecuteNegate()
    {
        auto val = ExtractValue<DoubleRepr>(StackPop());
        
        Value newVal(-val);
        
        StackPush(newVal);
    }
    
    void VirtualMachine::ExecuteEqual()
    {
        Value b = StackPop();
        Value a = StackPop();
        
        StackPush(Value(a == b));
    }
    
    void VirtualMachine::ExecuteGreater()
    {
        BinaryOperation<std::greater<DoubleRepr>>();
    }
    
    void VirtualMachine::ExecuteLess()
    {
        BinaryOperation<std::less<DoubleRepr>>();
    }
    
    void VirtualMachine::ExecutePop()
    {
        StackPop();
    }
    
    void VirtualMachine::ExecuteDefineGlobal()
    {
        // We can pop the value from the stack, because resizing the unordered map does not trigger the GC.
        Value val = StackPop();
        GcPtr<String> str = ReadStringArgument();
        
        globals.emplace(str, val);
    }
    
    void VirtualMachine::ExecuteGetGlobal()
    {
        GcPtr<String> str = ReadStringArgument();
        
        auto it = globals.find(str);
        if (it != globals.end())
        {
            Value val = it->second;
            StackPush(val);
        }
        else
        {
            ThrowRuntimeException<Exceptions::UndefinedVariable>(str);
        }
    }
    
    void VirtualMachine::ExecuteSetGlobal()
    {
        GcPtr<String> str = ReadStringArgument();
        
        Value val = StackPeek();
        
        auto it = globals.find(str);
        if (it != globals.end())
        {
            it->second = val;
        }
        else
        {
            ThrowRuntimeException<Exceptions::UndefinedVariable>(str);
        }
    }
    
    void VirtualMachine::ExecuteGetLocal()
    {
        uint8_t offset = CallFramePeek().ReadByte();
        
        StackPush(StackGet(offset));
    }
    
    void VirtualMachine::ExecuteSetLocal()
    {
        uint8_t offset = CallFramePeek().ReadByte();
        
        StackSet(offset, StackPeek());
    }
    
    void VirtualMachine::ExecuteJump()
    {
        uint16_t jump = CallFramePeek().ReadShort();
        
        CallFramePeek().Jump(jump);
    }
    
    void VirtualMachine::ExecuteJumpIfFalse()
    {
        uint16_t jump = CallFramePeek().ReadShort();
        
        if (StackPeek().IsFalse())
        {
            CallFramePeek().Jump(jump);
        }
    }
    
    void VirtualMachine::ExecuteLoop()
    {
        uint16_t jump = CallFramePeek().ReadShort();
        
        CallFramePeek().Jump(-jump);
    }
    
    void VirtualMachine::ExecuteCall()
    {
        uint8_t argCount = CallFramePeek().ReadByte();
        
        CallValue(StackPeek(argCount), argCount);
    }
    
    void VirtualMachine::ExecuteGetUpvalue()
    {
        CallFrame& frame = CallFramePeek();
        GcPtr<Closure> func = frame.GetFunction();
        
        uint8_t index = frame.ReadByte();
        
        if (!func->HasUpvalue(index))
        {
            ThrowRuntimeException<Exceptions::RuntimeException>("invalid upvalue index");
        }
        
        StackPush(frame.GetFunction()->GetUpvalue(index)->GetValue());
    }
    
    void VirtualMachine::ExecuteSetUpvalue()
    {
        CallFrame& frame = CallFramePeek();
        GcPtr<Closure> func = frame.GetFunction();
        
        uint8_t index = frame.ReadByte();
        
        if (!func->HasUpvalue(index))
        {
            ThrowRuntimeException<Exceptions::RuntimeException>("invalid upvalue index");
        }
        
        func->GetUpvalue(index)->SetUpvalue(StackPeek());
    }
    
    void VirtualMachine::ExecuteFillUpvalues()
    {
        CallFrame& frame = CallFramePeek();
        
        GcPtr<Closure> func = ExtractObject<Closure>(StackPeek());
        
        uint8_t count = frame.ReadByte();
        
        for (int i = 0; i < count; ++i)
        {
            uint8_t isLocal = frame.ReadByte();
            uint8_t index = frame.ReadByte();
            
            if (isLocal)
            {
                auto it = frame.GetValues() + index;
                func->AddUpValue(AddLocalUpvalue(it));
            }
            else
            {
                GcPtr<Closure> parent = CallFramePeek(1).GetFunction();
                
                if (!parent->HasUpvalue(index))
                {
                    ThrowRuntimeException<Exceptions::RuntimeException>("invalid upvalue index");
                }
                
                func->AddUpValue(func->GetUpvalue(index));
            }
        }
    }
    
    void VirtualMachine::ExecuteCloseUpvalue()
    {
        CloseUpvalues(StackPeekIterator());
        StackPop();
    }
    
    void VirtualMachine::ExecuteClass()
    {
        GcPtr<String> name = ReadStringArgument();
        
        auto obj = AllocateObject<Class>(name);
        
        StackPush(Value(obj));
    }
    
    void VirtualMachine::ExecuteGetProperty()
    {
        GcPtr<String> name = ReadStringArgument();
        
        auto obj = ExtractObject<Instance>(StackPeek());
        
        std::optional<Value> field = obj->GetField(name);
        
        if (field)
        {
            StackPop();
            StackPush(*field);
            return;
        }
        
        GcPtr<BoundMethod> bound = BindMethod(obj->GetClass(), name, obj);
        
        StackPop();
        StackPush(Value(bound));
    }
    
    void VirtualMachine::ExecuteSetProperty()
    {
        GcPtr<String> name = ReadStringArgument();
        
        auto obj = ExtractObject<Instance>(StackPeek(1));
        
        obj->SetField(name, StackPeek());
        
        Value val = StackPop();
        StackPop();
        StackPush(val);
    }
    
    void VirtualMachine::ExecuteMethod()
    {
        GcPtr<String> name = ReadStringArgument();
        
        auto method = ExtractObject<Closure>(StackPop());
        auto klass = ExtractObject<Class>(StackPeek());
        
        klass->AddMethod(name, method);
    }
    
    void VirtualMachine::ExecuteInvoke()
    {
        uint8_t argCount = CallFramePeek().ReadByte();
        
        GcPtr<String> name = ReadStringArgument();
        
        auto obj = ExtractObject<Instance>(StackPeek(argCount));
        
        Invoke(obj, name, argCount);
    }
    
    void VirtualMachine::ExecuteInherit()
    {
        auto to = ExtractObject<Class>(StackPeek());
        auto from = ExtractObject<Class>(StackPeek(1));
        
        to->Inherit(from);
        
        StackPop();
    }
    
    void VirtualMachine::ExecuteGetSuper()
    {
        auto thisObj = ExtractObject<Instance>(StackPeek(1));
        auto superObj = ExtractObject<Class>(StackPeek());
        
        GcPtr<String> name = ReadStringArgument();
        
        GcPtr<BoundMethod> bound = BindMethod(superObj, name, thisObj);
        
        StackPop();
        StackPop();
        StackPush(Value(bound));
    }
    
    void VirtualMachine::ExecuteInvokeSuper()
    {
        uint8_t argsCount = CallFramePeek().ReadByte();
        GcPtr<String> name = ReadStringArgument();
        
        GcPtr<Class> super = ExtractObject<Class>(StackPop());
        
        InvokeFromClass(super, name, argsCount);
    }
    
    // Runtime operations.
    
    void VirtualMachine::CloseUpvalues(StackIterator it)
    {
        while (!openUpvalues.IsNullptr()
               && openUpvalues->GetSlot() >= it)
        {
            GcPtr<Upvalue> upvalue = openUpvalues;
            GcPtr<Upvalue> next = openUpvalues->GetNextUpvalue();
            
            upvalue->Close();
            
            openUpvalues = next;
        }
    }
    
    void VirtualMachine::CallValue(Value val, uint8_t argCount)
    {
        auto obj = ExtractValue<ObjectRepr>(val);
        
        switch (obj->GetType())
        {
        case ObjectType::Closure:
            CallFunction(obj->As<Closure>(), argCount);
            break;
        case ObjectType::Native:
            CallNative(obj->As<Native>(), argCount);
            break;
        case ObjectType::Class:
            CallClass(obj->As<Class>(), argCount);
            break;
        case ObjectType::BoundMethod:
            CallBoundMethod(obj->As<BoundMethod>(), argCount);
            break;
        default:
            ThrowRuntimeException<Exceptions::NonCallable>(val);
            break;
        }
    }
    
    void VirtualMachine::CallFunction(GcPtr<Closure> func, uint8_t argCount)
    {
        if (func->GetArity() != argCount)
        {
            ThrowRuntimeException<Exceptions::WrongArgumentsCount>(func->GetArity(), argCount);
        }
        
        CallFramePush(CallFrame(func, StackPeekIterator(argCount)));
    }
    
    void VirtualMachine::CallNative(GcPtr<Native> func, uint8_t argCount)
    {
        if (func->GetArity() != argCount)
        {
            ThrowRuntimeException<Exceptions::WrongArgumentsCount>(func->GetArity(), argCount);
        }
        
        NativeFn& fn = func->GetNativeFn();
        Value val = (func->GetNativeFn())(*this, StackPeekIterator(argCount));
        StackPush(val);
    }
    
    void VirtualMachine::CallClass(GcPtr<Class> klass, uint8_t argCount)
    {
        auto obj = AllocateObject<Instance>(klass);
        
        std::optional<GcPtr<Closure>> init = klass->GetMethod(initStr);
        if (init)
        {
            CallFunction(*init, argCount);
        }
        else if (argCount != 0)
        {
            ThrowRuntimeException<Exceptions::WrongArgumentsCount>(0, argCount);
        }
        
        *StackPeekIterator(argCount) = Value(obj);
    }
    
    void VirtualMachine::CallBoundMethod(GcPtr<BoundMethod> method, uint8_t argCount)
    {
        // QUESTION: Is it safe to replace the bound method on the stack?
        // Like, it is gone and what if GC collects it?
        // Actually, even if GC deletes it, the reciever and all methods will be saved.
        // I think even simple or understandable questions like this are very important.
        
        *StackPeekIterator(argCount) = Value(method->GetReciever());
        CallFunction(method->GetMethod(), argCount);
    }
    
    void VirtualMachine::Invoke(GcPtr<Instance> obj, GcPtr<String> name, uint8_t argCount)
    {
        std::optional<Value> field = obj->GetField(name);
        
        if (field)
        {
            CallValue(*field, argCount);
        }
        else
        {
            InvokeFromClass(obj->GetClass(), name, argCount);
        }
    }
    
    void VirtualMachine::InvokeFromClass(GcPtr<Class> obj, GcPtr<String> name, uint8_t argCount)
    {
        std::optional<GcPtr<Closure>> method = obj->GetMethod(name);
        
        if (method)
        {
            CallFunction(*method, argCount);
        }
        else
        {
            ThrowRuntimeException<Exceptions::UndefinedProperty>(name);
        }
    }
    
    GcPtr<BoundMethod> VirtualMachine::BindMethod(GcPtr<Class> klass, GcPtr<String> name, GcPtr<Instance> obj)
    {
        std::optional<GcPtr<Closure>> method = klass->GetMethod(name);
        
        if (!method)
        {
            ThrowRuntimeException<Exceptions::UndefinedProperty>(name);
        }
        
        return AllocateObject<BoundMethod>(obj, *method);
    }
    
    GcPtr<Upvalue> VirtualMachine::AddLocalUpvalue(StackIterator it)
    {
        GcPtr<Upvalue> list = openUpvalues;
        GcPtr<Upvalue> prev(nullptr);
        
        while (!list.IsNullptr() && list->GetSlot() > it)
        {
            prev = list;
            list = list->GetNextUpvalue();
        }
        
        if (!list.IsNullptr() && list->GetSlot() == it)
        {
            return list;
        }
        
        auto newUpvalue = AllocateObject<Upvalue>(list, it);
        
        if (!prev.IsNullptr())
        {
            prev->SetNextUpvalue(newUpvalue);
        }
        else
        {
            openUpvalues = newUpvalue;
        }
        
        return newUpvalue;
    }
    
    template <typename T>
    T VirtualMachine::ExtractValue(Value val)
    {
        ValueType type = TypeToValueType<T>();
        if (!val.Is(type))
        {
            ThrowRuntimeException<Exceptions::WrongTypeValue>(val, type);
        }
        
        return val.As<T>();
    }
    
    template <typename T>
    GcPtr<T> VirtualMachine::ExtractObject(Value val)
    {
        if (!val.IsObject<T>())
        {
            ThrowRuntimeException<Exceptions::WrongTypeObject>(val, T::GetStaticType());
        }
        
        return val.AsObject<T>();
    }
    
    template <typename BinOp>
    void VirtualMachine::BinaryOperation()
    {
        Value b = StackPop();
        Value a = StackPop();
        
        if (std::is_same_v<BinOp, std::plus<DoubleRepr>> && a.IsObject<String>() && b.IsObject<String>())
        {
            GcPtr<String> str1 = a.AsObject<String>();
            GcPtr<String> str2 = b.AsObject<String>();
            StackPush(Value(Concatenate(str1, str2)));
            return;
        }
        
        auto aDouble = ExtractValue<DoubleRepr>(a);
        auto bDouble = ExtractValue<DoubleRepr>(b);
        
        if constexpr (std::is_same_v<BinOp, std::divides<DoubleRepr>>)
        {
            if (bDouble == 0)
            {
                ThrowRuntimeException<Exceptions::ZeroDivision>();
            }
        }
        
        StackPush(Value(BinOp{}(aDouble, bDouble)));
    }
    
    GcPtr<String> VirtualMachine::Concatenate(GcPtr<String> a, GcPtr<String> b)
    {
        const std::string& str1 = a->GetCppString();
        const std::string& str2 = b->GetCppString();
        
        std::string newStr = str1 + str2;
        
        return AllocateObject<String>(std::move(newStr));
    }
    
    // Stacks.
    
    void VirtualMachine::CallFramePush(CallFrame&& frame)
    {
        if (framesCount == Configuration::FramesCount)
        {
            // The same question as in `CallFramePop`.
            ThrowRuntimeException<Exceptions::StackOverflow>();
        }
        
        frames[framesCount++] = frame;
    }
    
    void VirtualMachine::CallFramePop()
    {
        if (framesCount == 0)
        {
            // TODO: Stack underflow or another error?
            // Probably another error, but I am too lazy.
            ThrowRuntimeException<Exceptions::StackUnderflow>();
        }
        
        stackTop = CallFramePeek().GetValues();
        framesCount--;
    }
    
    CallFrame& VirtualMachine::CallFramePeek(std::size_t offset)
    {
        return frames[framesCount - offset - 1];
    }
    
    const CallFrame& VirtualMachine::CallFrameTop() const
    {
        return frames[framesCount - 1];
    }
    
    void VirtualMachine::StackPush(Value val)
    {
        if (stackTop == stack.end())
        {
            ThrowRuntimeException<Exceptions::StackOverflow>();
        }
        
        *stackTop = val;
        ++stackTop;
    }
    
    Value VirtualMachine::StackPop()
    {
        if (stackTop == stack.begin())
        {
            ThrowRuntimeException<Exceptions::StackUnderflow>();
        }
        
        return *--stackTop;
    }
    
    Value VirtualMachine::StackPeek(std::size_t offset)
    {
        return *StackPeekIterator(offset);
    }
    
    StackIterator VirtualMachine::StackPeekIterator(std::size_t offset)
    {
        if (stackTop - offset < stack.begin())
        {
            ThrowRuntimeException<Exceptions::StackUnderflow>();
        }
        
        return stackTop - offset - 1;
    }
    
    Value VirtualMachine::StackGet(std::size_t index)
    {
        auto values = CallFramePeek().GetValues();
        
        if (values + index >= stack.end())
        {
            ThrowRuntimeException<Exceptions::InvalidStackAccess>();
        }
        
        return values[index];
    }
    
    void VirtualMachine::StackSet(std::size_t index, Value newVal)
    {
        auto values = CallFramePeek().GetValues();
        
        if (values + index >= stack.end())
        {
            ThrowRuntimeException<Exceptions::InvalidStackAccess>();
        }
        
        values[index] = newVal;
    }
    
    // Debug.
    
    void VirtualMachine::TraceExecution(std::ostream& out) const
    {
        PrintStack(out);
        
        GcPtr<const Closure> func = CallFrameTop().GetFunction();
        GcPtr<const String> name = func->GetName();
        
        ChunkDumper dumper(func->GetChunk(),
                           name.IsNullptr() ? "<script>" : name->GetCppString(),
                           out);
        dumper.DisassembleInstruction(CallFrameTop().GetIP());
    }
    
    void VirtualMachine::PrintStack(std::ostream& out) const
    {
        for (auto it = stack.begin(); it != stackTop; ++it)
        {
            out << "[ " << *it << " ] ";
        }
        
        if (stack.empty())
        {
            out << "# empty-stack #";
        }
        
        out << std::endl;
    }
    
    // Errors.
    
    std::vector<Exceptions::StackTraceEntry> VirtualMachine::MakeStackTrace() const
    {
        std::vector<Exceptions::StackTraceEntry> stackTrace;
        
        for (std::size_t i = 0; i < framesCount; ++i)
        {
            const CallFrame& frame = frames[i];
            stackTrace.emplace_back(frame.GetFunction(), frame.GetIP());
        }
        
        return stackTrace;
    }
    
    // Memory.
    
    void VirtualMachine::MarkRoots()
    {
        for (auto& entry: strings)
        {
            MemoryManager::MarkObject(entry.second);
        }
        
        for (auto& entry: globals)
        {
            // NOTE: Keys in unordered map are const.
            
            // So, there is no other way
            MemoryManager::MarkObject(static_cast<GcPtr<String>>(entry.first));
            MemoryManager::MarkValue(entry.second);
        }
        
        GcPtr<Upvalue> upvalue = openUpvalues;
        while (!upvalue.IsNullptr())
        {
            GcPtr<Upvalue> next = upvalue->GetNextUpvalue();
            
            MemoryManager::MarkObject(upvalue);
            
            upvalue = next;
        }
        
        for (std::size_t i = 0; i < framesCount; ++i)
        {
            CallFrame& frame = frames[i];
            MemoryManager::MarkObject(frame.GetFunction());
        }
        
        for (auto it = stack.begin(); it < stackTop; ++it)
        {
            MemoryManager::MarkValue(*it);
        }
    }
}
