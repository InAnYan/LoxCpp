#ifndef LOX_VM_INTERPRETER_VIRTUAL_MACHINE_CONFIGURATION_HPP
#define LOX_VM_INTERPRETER_VIRTUAL_MACHINE_CONFIGURATION_HPP

#include <iostream>

namespace Lox
{
    class VirtualMachineConfiguration
    {
    public:
        VirtualMachineConfiguration(std::ostream& userOutput, std::istream& userInput, std::ostream& debugOutput);
        
        std::ostream& GetUserOutput() const;
        
        std::istream& GetUserInput() const;
        
        std::ostream& GetDebugOutput() const;
    
    private:
        std::ostream& userOutput;
        std::istream& userInput;
        std::ostream& debugOutput;
    }; // class VirtualMachineConfiguration
}

#endif // LOX_VM_INTERPRETER_VIRTUAL_MACHINE_CONFIGURATION_HPP
