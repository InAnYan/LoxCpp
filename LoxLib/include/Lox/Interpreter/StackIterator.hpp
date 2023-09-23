#ifndef LOX_VM_INTERPRETER_STACK_ITERATOR_HPP
#define LOX_VM_INTERPRETER_STACK_ITERATOR_HPP

#include <array>

#include "Lox/Configuration.hpp"

#include "Lox/Runtime/Value.hpp"

namespace Lox
{
    using StackIterator = std::array<Value, Configuration::FramesCount>::iterator;
}

#endif // LOX_VM_INTERPRETER_STACK_ITERATOR_HPP
