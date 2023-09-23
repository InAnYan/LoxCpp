#include "ClassCompiler.hpp"

#include <utility>

namespace Lox
{
    bool ClassCompiler::HasSuper() const
    {
        return hasSuper;
    }
    
    void ClassCompiler::SetSuper(bool val)
    {
        hasSuper = val;
    }
}