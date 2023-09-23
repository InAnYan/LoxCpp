#ifndef LOX_VM_COMPILER_PRECEDENCE_HPP
#define LOX_VM_COMPILER_PRECEDENCE_HPP

namespace Lox
{
    enum Precedence
    {
        PrecedenceNone,
        PrecedenceOr,
        PrecedenceAnd,
        PrecedenceAssignment,
        PrecedenceEquality,
        PrecedenceComparison,
        PrecedenceTerm,
        PrecedenceFactor,
        PrecedenceUnary,
        PrecedenceCall,
        PrecedencePrimary
    };
}

#endif // LOX_VM_COMPILER_PRECEDENCE_HPP
