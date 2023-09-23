#ifndef LOX_VM_COMPILER_SCANNER_HPP
#define LOX_VM_COMPILER_SCANNER_HPP

#include <functional>
#include <string_view>
#include <unordered_map>

#include "TokenType.hpp"
#include "Token.hpp"
#include "Lox/Compiler/SourcePosition.hpp"

namespace Lox
{
    class Scanner
    {
    public:
        Scanner(std::string_view filePath, std::string_view input);

        Token NextToken();

        Token operator++();
        Token operator++(int);
        
        std::string_view GetFilePath() const;
            
    private:
        std::string_view filePath;

        std::string::const_iterator start;
        std::string::const_iterator current;
        std::string::const_iterator inputEnd;

        std::size_t line;

        void SkipWhiteSpace();
        void BeginNewToken();

        char Advance();
        void AdvanceWhile(const std::function<bool(char)>& predicate);
        bool Match(char ch);

        Token ReadNumber();
        Token ReadIdentifierOrKeyword();
        Token ReadString();
        
        TokenType IdentifierOrKeyword();
        std::unordered_map<std::string_view, TokenType> keywordsMap;

        Token MakeToken(TokenType type);
        Token MakeErrorToken(std::string_view errorMsg);
        SourcePosition MakeSourcePosition();

        bool IsAtEnd() const;
        char Peek(std::size_t offset = 0) const;
    }; // class Scanner
}

#endif // LOX_VM_COMPILER_SCANNER_HPP
