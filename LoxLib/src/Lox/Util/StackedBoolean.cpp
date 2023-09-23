#include "Lox/Util/StackedBoolean.hpp"

#include <limits>

#include "Lox/Util/Assert.hpp"

namespace Lox
{
    StackedBoolean::StackedBoolean()
        : impl(0)
    {
    
    }
    
    StackedBoolean::operator bool() const
    {
        return impl != 0;
    }
    
    void StackedBoolean::On()
    {
        // QUESTION: Actually, it shouldn't be an assert?
        LOX_ASSERT(impl != std::numeric_limits<unsigned>::max(), "Lox::StackedBoolean too many ons");
        ++impl;
    }
    
    void StackedBoolean::Off()
    {
        // Of course, the class could just do nothing.
        LOX_ASSERT(impl != 0, "Lox::StackedBoolean offing zero boolean");
        --impl;
    }
}