#include "Scanner.hpp"

#include "Lox/Compiler/SourcePosition.hpp"
#include "Lox/Util/Assert.hpp"

namespace Lox
{
    bool IsDigit(char ch);
    
    bool IsAlpha(char ch);
    
    Scanner::Scanner(std::string_view filePath, std::string_view input)
            : filePath(filePath), start(input.begin()), current(start), inputEnd(input.end()), line(1)
    {
        #define LOX_KEYWORDS_FILL_MAP(name, type) \
            keywordsMap[#name] = TokenType::type;
        
        LOX_KEYWORDS_LIST(LOX_KEYWORDS_FILL_MAP);
        
        #undef LOX_KEYWORDS_FILL_MAP
    }
    
    std::string_view Scanner::GetFilePath() const
    {
        return filePath;
    }
    
    Token Scanner::NextToken()
    {
        SkipWhiteSpace();
        BeginNewToken();
        
        if (IsAtEnd())
        {
            return MakeToken(TokenType::EndOfFile);
        }
        
        char ch = Advance();
        switch (ch)
        {
        case '+':
            return MakeToken(TokenType::Plus);
        case '-':
            return MakeToken(TokenType::Minus);
        case '*':
            return MakeToken(TokenType::Star);
        case '/':
            return MakeToken(TokenType::Slash);
        
        case '(':
            return MakeToken(TokenType::LeftParenthesis);
        case ')':
            return MakeToken(TokenType::RightParenthesis);
        
        case '{':
            return MakeToken(TokenType::LeftBracket);
        case '}':
            return MakeToken(TokenType::RightBracket);
        
        case ';':
            return MakeToken(TokenType::Semicolon);
        case ',':
            return MakeToken(TokenType::Comma);
        case '.':
            return MakeToken(TokenType::Dot);
        
        case '=':
            return MakeToken(Match('=') ? TokenType::EqualEqual : TokenType::Equal);
        
        case '!':
            return MakeToken(Match('=') ? TokenType::BangEqual : TokenType::Bang);
        
        case '>':
            return MakeToken(Match('=') ? TokenType::GreaterEqual : TokenType::Greater);
        
        case '<':
            return MakeToken(Match('=') ? TokenType::LessEqual : TokenType::Less);
        
        case '"':
            return ReadString();
        
        default:
            if (IsDigit(ch))
            {
                return ReadNumber();
            }
            else if (IsAlpha(ch) || ch == '_')
            {
                return ReadIdentifierOrKeyword();
            }
            else
            {
                return MakeErrorToken("unknown character");
            }
        }
    }
    
    Token Scanner::operator++()
    {
        return NextToken();
    }
    
    Token Scanner::operator++(int)
    {
        return NextToken();
    }
    
    void Scanner::SkipWhiteSpace()
    {
        while (true)
        {
            switch (Peek())
            {
            case '\n':
                line++;
            case ' ':
            case '\t':
            case '\r':
                Advance();
                continue;
            case '/':
                if (Peek(1) == '/')
                {
                    AdvanceWhile([](char ch)
                                 {
                                     return ch != '\n';
                                 });
                    continue;
                }
                return;
            default:
                return;
            }
        }
    }
    
    void Scanner::BeginNewToken()
    {
        start = current;
    }
    
    Token Scanner::ReadNumber()
    {
        AdvanceWhile(IsDigit);
        
        if (Match('.'))
        {
            AdvanceWhile(IsDigit);
        }
        
        return MakeToken(TokenType::Number);
    }
    
    Token Scanner::ReadIdentifierOrKeyword()
    {
        AdvanceWhile([](char ch)
                     {
                         return IsAlpha(ch) || IsDigit(ch) || ch == '_';
                     });
        
        return MakeToken(IdentifierOrKeyword());
    }
    
    Token Scanner::ReadString()
    {
        AdvanceWhile([](char ch)
                     {
                         return ch != '"';
                     });
        
        if (IsAtEnd())
        {
            return MakeErrorToken("unterminated string");
        }
        
        Advance();
        
        return MakeToken(TokenType::String);
    }
    
    TokenType Scanner::IdentifierOrKeyword()
    {
        auto it = keywordsMap.find(std::string_view(start, current));
        
        if (it != keywordsMap.end())
        {
            return it->second;
        }
        else
        {
            return TokenType::Identifier;
        }
    }
    
    char Scanner::Advance()
    {
        LOX_ASSERT(!IsAtEnd(), "attempt to advance while being at the end of the input");
        current++;
        return current[-1];
    }
    
    void Scanner::AdvanceWhile(const std::function<bool(char)>& predicate)
    {
        while (!IsAtEnd() && predicate(Peek()))
        {
            Advance();
        }
    }
    
    bool Scanner::Match(char ch)
    {
        if (Peek() != ch)
        {
            return false;
        }
        
        Advance();
        return true;
    }
    
    Token Scanner::MakeToken(TokenType type)
    {
        return Token(type, MakeSourcePosition(), std::string_view(start, current));
    }
    
    Token Scanner::MakeErrorToken(std::string_view errorMsg)
    {
        return Token(TokenType::Error, MakeSourcePosition(), errorMsg);
    }
    
    SourcePosition Scanner::MakeSourcePosition()
    {
        return SourcePosition(line);
    }
    
    bool Scanner::IsAtEnd() const
    {
        return current == inputEnd;
    }
    
    char Scanner::Peek(std::size_t offset) const
    {
        return current[offset];
    }
    
    /// Fully inclusive range.
    bool CharInRange(char ch, char a, char b);
    
    bool IsDigit(char ch)
    {
        return CharInRange(ch, '0', '9');
    }
    
    bool IsAlpha(char ch)
    {
        return CharInRange(ch, 'a', 'z')
               || CharInRange(ch, 'A', 'Z');
    }
    
    bool CharInRange(char ch, char a, char b)
    {
        return ch >= a && ch <= b;
    }
}
