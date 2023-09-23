#ifndef LOX_VM_UTIL_OPTIONAL_REFERENCE_HPP
#define LOX_VM_UTIL_OPTIONAL_REFERENCE_HPP

#include <optional>
#include <functional>

namespace Lox
{
    template <typename T>
    using OptionalReference = std::optional<const std::reference_wrapper<T>>;
}

#endif // LOX_VM_UTIL_OPTIONAL_REFERENCE_HPP
