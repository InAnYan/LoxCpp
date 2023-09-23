#ifndef LOX_VM_STAGES_EXCEPTIONS_OPTIMIZER_FAIL_HPP
#define LOX_VM_STAGES_EXCEPTIONS_OPTIMIZER_FAIL_HPP

#include <stdexcept>
#include <ostream>

namespace Lox
{
    namespace Exceptions
    {
        class OptimizerFailure : public std::runtime_error
        {
        public:
            OptimizerFailure(const char* cause);
        };
        
        std::ostream& operator<<(std::ostream& out, const OptimizerFailure& e);
    }
}

#endif // LOX_VM_STAGES_EXCEPTIONS_OPTIMIZER_FAIL_HPP
