#include "ParserRule.hpp"

namespace Lox
{
    ParserRule::ParserRule()
            : prefix(nullptr), infix(nullptr), prec(PrecedenceNone)
    {
        
    }
    
    ParserRule::ParserRule(ParseFn prefix, ParseFn infix, Precedence prec)
            : prefix(prefix), infix(infix), prec(prec)
    {
        
    }
    
    ParseFn ParserRule::GetPrefix() const
    {
        return prefix;
    }
    
    ParseFn ParserRule::GetInfix() const
    {
        return infix;
    }
    
    Precedence ParserRule::GetPrecedence() const
    {
        return prec;
    }
}