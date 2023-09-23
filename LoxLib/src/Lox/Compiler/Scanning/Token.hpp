#ifndef LOX_VM_COMPILER_TOKEN_HPP
#define LOX_VM_COMPILER_TOKEN_HPP

#include <string_view>

#include "Lox/Compiler/SourcePosition.hpp"
#include "TokenType.hpp"

namespace Lox
{
    class Token
    {
    public:
        /// Undefined token.
        Token();

        /// Token from source.
        Token(TokenType type, SourcePosition pos, std::string_view str);
        
        /// Synthetic token.
        Token(std::string_view str);
            
        TokenType GetType() const;
        std::string_view GetStr() const;
        SourcePosition GetPos() const;
            
    private:
        TokenType type;
        SourcePosition pos;
        std::string_view str;
    }; // class Token
}

#endif // LOX_VM_COMPILER_TOKEN_HPP
