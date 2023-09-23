#include "Lox/Util/Assert.hpp"

#include <iostream>
#include <cstdlib>

#include "Lox/Configuration.hpp"

namespace Lox
{
    void Assert(bool expr, std::string_view exprStr, std::string_view errorMsg, int line, std::string_view fileName)
    {
        if constexpr (!Configuration::DebugMode)
        {
            return;
        }

        if (expr)
        {
            return;
        }

        std::cerr << "ASSERTION FAILED: " << exprStr << std::endl;
        std::cerr << "Message: " << errorMsg << std::endl;
        std::cerr << "Location: " << fileName << ":" << line << std::endl;

        std::abort();
    }
}
