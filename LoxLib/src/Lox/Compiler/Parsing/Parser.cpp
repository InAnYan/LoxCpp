#include "Parser.hpp"

namespace Lox
{
    Parser::Parser(CompilerErrorReporter& errorReporter, std::string_view filePath, std::string_view input)
            : scanner(filePath, input), hadError(false), errorReporter(errorReporter)
    {
    
    }
    
    Token Parser::GetCurrent() const
    {
        return current;
    }
    
    Token Parser::GetNext() const
    {
        return next;
    }
    
    std::string_view Parser::GetFilePath() const
    {
        return scanner.GetFilePath();
    }
    
    bool Parser::IsAtEnd() const
    {
        return next.GetType() == TokenType::EndOfFile;
    }
    
    bool Parser::CheckCurrent(TokenType type) const
    {
        return current.GetType() == type;
    }
    
    bool Parser::CheckNext(TokenType type) const
    {
        return next.GetType() == type;
    }
    
    void Parser::Advance()
    {
        current = next;
        
        while (true)
        {
            next = ++scanner;
            
            if (next.GetType() == TokenType::Error)
            {
                errorReporter.Error(next.GetPos(), next.GetStr());
            }
            else
            {
                break;
            }
        }
    }
    
    Token Parser::Require(TokenType type, std::string_view errorMsg)
    {
        if (Match(type))
        {
            return current;
        }
        
        Advance();
        
        ErrorAtCurrent(errorMsg);
        return Token(); // To suppress warning.
    }
    
    void Parser::ErrorAtNext(std::string_view msg)
    {
        ErrorAt(next.GetPos(), msg);
    }
    
    void Parser::ErrorAtCurrent(std::string_view msg)
    {
        ErrorAt(current.GetPos(), msg);
    }
    
    void Parser::ErrorAt(SourcePosition pos, std::string_view msg)
    {
        errorReporter.Error(pos, msg);
        hadError = true;
        throw ParserException();
    }
    
    bool Parser::HadError() const
    {
        return hadError;
    }
}