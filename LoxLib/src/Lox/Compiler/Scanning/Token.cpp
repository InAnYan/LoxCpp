#include "Token.hpp"
#include "Lox/Compiler/SourcePosition.hpp"

namespace Lox
{
    Token::Token()
            : type(TokenType::Undefined), pos(SourcePosition(0)), str("unintialized token")
    {
    
    }
    
    Token::Token(TokenType type, SourcePosition pos, std::string_view str)
            : type(type), pos(pos), str(str)
    {
    
    }
    
    Token::Token(std::string_view str)
            : type(TokenType::Synthetic), pos(SourcePosition(0)), str(str)
    {
    
    }
    
    TokenType Token::GetType() const
    {
        return type;
    }
    
    std::string_view Token::GetStr() const
    {
        return str;
    }
    
    SourcePosition Token::GetPos() const
    {
        return pos;
    }
}
