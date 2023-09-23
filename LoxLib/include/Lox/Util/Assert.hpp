#ifndef LOX_VM_UTIL_ASSERT_HPP
#define LOX_VM_UTIL_ASSERT_HPP

#include <string_view>

#define LOX_ASSERT(expr, errorMsg) \
    Lox::Assert((expr), (#expr), (errorMsg), __LINE__, __FILE__)

#define LOX_UNREACHABLE(errorMsg) \
    Lox::Assert(false, "false", "Unreachable: " errorMsg, __LINE__, __FILE__)

#define LOX_FAIL(errorMsg) \
    Lox::Assert(false, "false", "Failure: " errorMsg, __LINE__, __FILE__)

#define LOX_UNIMPLEMENTED(errorMsg) \
    Lox::Assert(false, "false", "Unimplemented: " errorMsg, __LINE__, __FILE__)

namespace Lox
{
    void Assert(bool expr, std::string_view exprStr, std::string_view errorMsg, int line, std::string_view fileName);
}

#endif // LOX_VM_UTIL_ASSERT_HPP
