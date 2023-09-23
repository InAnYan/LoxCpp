#ifndef LOX_VM_COMPILER_COMPILERS_CLASS_COMPILER_HPP
#define LOX_VM_COMPILER_COMPILERS_CLASS_COMPILER_HPP

#include "Lox/Util/OptionalReference.hpp"

namespace Lox
{
    class ClassCompiler
    {
    public:
        bool HasSuper() const;
        void SetSuper(bool val);
        
    private:
        bool hasSuper;
    };
}

#endif // LOX_VM_COMPILER_COMPILERS_CLASS_COMPILER_HPP
