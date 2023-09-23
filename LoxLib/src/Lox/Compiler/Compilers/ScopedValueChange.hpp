#ifndef LOX_VM_COMPILER_COMPILERS_SCOPED_VALUE_CHANGE_HPP
#define LOX_VM_COMPILER_COMPILERS_SCOPED_VALUE_CHANGE_HPP

#include <functional>

namespace Lox
{
    template <typename T>
    class ScopedValueChange
    {
    public:
        ScopedValueChange(T&& newVal, T& ref)
            : oldVal(std::move(ref)), theRef(ref)
        {
            theRef = newVal;
        }
        
        ~ScopedValueChange()
        {
            theRef = std::move(oldVal);
        }
        
    private:
        T oldVal;
        T& theRef;
    };
}

#endif // LOX_VM_COMPILER_COMPILERS_SCOPED_VALUE_CHANGE_HPP
