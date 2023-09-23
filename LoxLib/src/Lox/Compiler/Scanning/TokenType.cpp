#include "TokenType.hpp"

namespace Lox    
{
    std::ostream& operator<<(std::ostream& out, const TokenType& type)
    {
#define LOX_PRINT_TOKEN_TYPE(name) case TokenType::name: out << #name; break;

        switch (type)
        {
            LOX_TOKEN_TYPE_LIST(LOX_PRINT_TOKEN_TYPE);
        }

        return out;
    }
}
