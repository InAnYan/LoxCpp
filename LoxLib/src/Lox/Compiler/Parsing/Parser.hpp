#ifndef LOX_VM_COMPILER_PARSER_HPP
#define LOX_VM_COMPILER_PARSER_HPP

#include <string_view>
#include <exception>

#include "../Scanning/Scanner.hpp"

#include "Lox/Compiler/CompilerErrorReporter.hpp"

namespace Lox
{
    class ParserException : public std::exception
    {
    
    }; // class ParserException
    
    class Parser
    {
    public:
        Parser(CompilerErrorReporter& errorReporter, std::string_view filePath, std::string_view input);
        
        Token GetCurrent() const;
        Token GetNext() const;
        
        std::string_view GetFilePath() const;
        
        bool IsAtEnd() const;
        
        void Advance();
        
        bool CheckCurrent(TokenType type) const;
        bool CheckNext(TokenType type) const;
        
        template <typename First>
        bool Match(First type)
        {
            if (next.GetType() != type)
            {
                return false;
            }
            
            Advance();
            return true;
        }
        
        template <typename First, typename Second, typename... Rest>
        bool Match(First type1, Second type2, Rest... restTypes)
        {
            return Match(type1) || Match(type2, restTypes...);
        }
        
        Token Require(TokenType type, std::string_view errorMsg);
        
        bool HadError() const;
        
        void ErrorAtNext(std::string_view msg);
        
        void ErrorAtCurrent(std::string_view msg);
        
        void ErrorAt(SourcePosition pos, std::string_view msg);
    
    private:
        Scanner scanner;
        
        bool hadError;
        CompilerErrorReporter& errorReporter;
        
        Token current;
        Token next;
    };
}

#endif // LOX_VM_COMPILER_PARSER_HPP
