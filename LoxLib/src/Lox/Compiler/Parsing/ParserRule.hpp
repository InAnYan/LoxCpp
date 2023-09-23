#ifndef LOX_VM_COMPILER_PARSER_RULE_HPP
#define LOX_VM_COMPILER_PARSER_RULE_HPP

#include "Precedence.hpp"

namespace Lox
{
    class Compiler;
    
    using ParseFn = void (Compiler::*)(bool canAssign);
    
    class ParserRule
    {
    public:
        ParserRule();
        
        ParserRule(ParseFn prefix, ParseFn infix, Precedence prec);
        
        ParseFn GetPrefix() const;
        
        ParseFn GetInfix() const;
        
        Precedence GetPrecedence() const;
    
    private :
        ParseFn prefix;
        ParseFn infix;
        Precedence prec;
    };
}

#endif // LOX_VM_COMPILER_PARSER_RULE_HPP
