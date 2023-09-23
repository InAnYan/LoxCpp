#ifndef LOX_VM_RUNTIME_NIL_TYPE_HPP
#define LOX_VM_RUNTIME_NIL_TYPE_HPP

#include <iostream>

namespace Lox
{
    /// A class that represents nil value. It is used for equality and printing overloads.
    class Nil
    {
    public:
        bool operator==(const Nil& other) const;
    }; // class Nil
    
    std::ostream& operator<<(std::ostream& out, const Nil& nil);
}

#endif // LOX_VM_RUNTIME_NIL_TYPE_HPP
