#ifndef LOX_VM_COMPILER_TOKEN_TYPE_HPP
#define LOX_VM_COMPILER_TOKEN_TYPE_HPP

#include <ostream>

#define LOX_TOKEN_TYPE_LIST(o)                  \
    o(Undefined)                                \
    o(Synthetic)                                \
    o(EndOfFile)                                \
    o(Error)                                    \
    o(Number)                                   \
    o(Plus)                                     \
    o(Minus)                                    \
    o(Star)                                     \
    o(Slash)                                    \
    o(Bang)                                     \
    o(LeftParenthesis)                          \
    o(RightParenthesis)                         \
    o(Nil)                                      \
    o(True)                                     \
    o(False)                                    \
    o(Identifier)                               \
    o(Greater)                                  \
    o(Less)                                     \
    o(Equal)                                    \
    o(BangEqual)                                \
    o(EqualEqual)                               \
    o(GreaterEqual)                             \
    o(LessEqual)                                \
    o(String)                                   \
    o(Print)                                    \
    o(Var)                                      \
    o(Semicolon)                                \
    o(LeftBracket)                              \
    o(RightBracket)                             \
    o(If)                                       \
    o(Else)                                     \
    o(And)                                      \
    o(Or)                                       \
    o(While)                                    \
    o(Break)                                    \
    o(Continue)                                 \
    o(Fun)                                      \
    o(Comma)                                    \
    o(Return)                                   \
    o(Class)                                    \
    o(Dot)                                      \
    o(This)                                     \
    o(Super)

#define LOX_KEYWORDS_LIST(o)  \
    o(nil, Nil)               \
    o(true, True)             \
    o(false, False)           \
    o(print, Print)           \
    o(var, Var)               \
    o(if, If)                 \
    o(else, Else)             \
    o(and, And)               \
    o(or, Or)                 \
    o(while, While)           \
    o(break, Break)           \
    o(continue, Continue)     \
    o(fun, Fun)               \
    o(return, Return)         \
    o(class, Class)           \
    o(this, This)             \
    o(super, Super)

namespace Lox
{
    enum class TokenType
    {
        #define LOX_MAKE_TOKEN_TYPE(name) name,
        
        LOX_TOKEN_TYPE_LIST(LOX_MAKE_TOKEN_TYPE)
        
        #undef LOX_MAKE_TOKEN_TYPE
    }; // enum class TokenType
    
    std::ostream& operator<<(std::ostream& out, const TokenType& type);
}

#endif // LOX_VM_COMPILER_TOKEN_TYPE_HPP
