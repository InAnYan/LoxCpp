#ifndef LOX_VM_INTERPRETER_EXCEPTIONS_WRONG_TYPE_HPP
#define LOX_VM_INTERPRETER_EXCEPTIONS_WRONG_TYPE_HPP

#include "RuntimeException.hpp"

#include "Lox/Runtime/ValueType.hpp"
#include "Lox/Runtime/Value.hpp"

namespace Lox::Exceptions
{
    template <typename T>
    class WrongType : public RuntimeException
    {
    public:
        WrongType(SourcePosition pos, std::vector<StackTraceEntry>&& stackTrace, Value gotValue, T expectedType)
                : RuntimeException(pos, std::move(stackTrace), "wrong type"), gotValue(gotValue),
                  expectedType(expectedType)
        {
            
        }
        
        Value GetGotValue() const
        {
            return gotValue;
        }
        
        T GetExpectedType() const
        {
            return expectedType;
        }
        
        std::ostream& PrintAdditional(std::ostream& out) const override
        {
            return out << ": got value " << gotValue << ", expected of type " << expectedType;
        }
    
    private:
        Value gotValue;
        T expectedType;
    };
    
    using WrongTypeValue = WrongType<ValueType>;
    using WrongTypeObject = WrongType<ObjectType>;
}

#endif // LOX_VM_INTERPRETER_EXCEPTIONS_WRONG_TYPE_HPP
