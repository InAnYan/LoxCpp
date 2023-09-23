#ifndef LOX_VM_UTIL_STACKED_BOOLEAN_HPP
#define LOX_VM_UTIL_STACKED_BOOLEAN_HPP

namespace Lox
{
    // Don't know how to explain this.
    class StackedBoolean
    {
    public:
        StackedBoolean();
        
        operator bool() const;
        
        void On();
        void Off();
        
    private:
        unsigned impl;
    };
}

#endif // LOX_VM_UTIL_STACKED_BOOLEAN_HPP
