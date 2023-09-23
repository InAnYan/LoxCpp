#include "Lox/Interpreter/VirtualMachineConfiguration.hpp"

namespace Lox
{
    VirtualMachineConfiguration::VirtualMachineConfiguration(std::ostream& userOutput, std::istream& userInput, std::ostream& debugOutput)
        : userOutput(userOutput), userInput(userInput), debugOutput(debugOutput)
    {
            
    }

    std::ostream& VirtualMachineConfiguration::GetUserOutput() const
    {
        return userOutput;
    }

    std::istream& VirtualMachineConfiguration::GetUserInput() const
    {
        return userInput;
    }

    std::ostream& VirtualMachineConfiguration::GetDebugOutput() const
    {
        return debugOutput;
    }
}
