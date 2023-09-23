#ifndef LOX_VM_RUNTIME_ROOTS_SOURCE_HPP
#define LOX_VM_RUNTIME_ROOTS_SOURCE_HPP

namespace Lox
{
    // Looked up from the Magpie (@munificent).
    
    // This is very elegant and simple approach.
    // But, what is using lambdas is more generic?
    
    class RootsSource
    {
    public:
        virtual void MarkRoots() = 0;
    };
}

#endif // LOX_VM_RUNTIME_ROOTS_SOURCE_HPP
