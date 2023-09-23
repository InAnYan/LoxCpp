#ifndef LOX_INTERPRETER_NATIVES_HPP
#define LOX_INTERPRETER_NATIVES_HPP

#include <Lox/Lox.hpp>

namespace Natives
{
    Lox::Value Clock(Lox::VirtualMachine& vm, Lox::StackIterator args);
    
    void AddNatives(Lox::VirtualMachine& vm);
}

#endif // LOX_INTERPRETER_NATIVES_HPP
