#include "Lox/Runtime/Nil.hpp"

namespace Lox    
{
    bool Nil::operator==([[maybe_unused]] const Nil& other) const
    {
        // There is no fields, so compiler can't create a default `operator==`.
        return true;
    }

    std::ostream& operator<<(std::ostream& out, const Nil& nil)
    {
        return out << "nil";
    }
}
