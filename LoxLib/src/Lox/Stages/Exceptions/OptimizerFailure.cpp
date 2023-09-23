#include "Lox/Stages/Exceptions/OptimizerFailure.hpp"

namespace Lox
{
    namespace Exceptions
    {
        OptimizerFailure::OptimizerFailure(const char* cause)
            : std::runtime_error(cause)
        {
        
        }
        
        std::ostream& operator<<(std::ostream& out, const OptimizerFailure& e)
        {
            return out << e.what();
        }
    }
}