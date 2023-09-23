#ifndef LOX_VM_UTIL_OVERLOADED_VISITOR_HPP
#define LOX_VM_UTIL_OVERLOADED_VISITOR_HPP

namespace Lox
{
    // SOURCE: https://en.cppreference.com/w/cpp/utility/variant/visit

    template<class... Ts>
    struct overloaded : Ts... { using Ts::operator()...; };

    template<class... Ts>
    overloaded(Ts...) -> overloaded<Ts...>;
}

#endif // LOX_VM_UTIL_OVERLOADED_VISITOR_HPP
