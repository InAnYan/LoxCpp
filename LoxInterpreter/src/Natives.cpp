#include "Natives.hpp"

#include <ctime>

namespace Natives
{
    Lox::Value Clock(Lox::VirtualMachine& vm, Lox::StackIterator args)
    {
        return Lox::Value(static_cast<Lox::DoubleRepr>(clock()) / CLOCKS_PER_SEC);
    }
    
    void AddNatives(Lox::VirtualMachine& vm)
    {
        vm.DefineNative("clock", 0, Clock);
    }
}