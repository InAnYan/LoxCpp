#include <gtest/gtest.h>

#include <Lox/Lox.hpp>

#include "../../LoxLib/src/Lox/Compiler/Scanning/TokenType.hpp"
#include "../../LoxLib/src/Lox/Compiler/Scanning/Token.hpp"
#include "../../LoxLib/src/Lox/Compiler/Scanning/Scanner.hpp"

using namespace Lox;

void GenericFirstTokenCheck(std::string_view input, TokenType actualType);

void GenericReadAllTokens(std::string_view input, std::initializer_list<Token> tokens);

SourcePosition MakePosition(std::size_t line);

TEST(ScannerTest, WhitespaceWithArithmetic)
{
    GenericReadAllTokens(" \t\n +   -  \r   \t  \t * \n\n/    \r",
                         { Token(TokenType::Plus, MakePosition(2), "+"),
                           Token(TokenType::Minus, MakePosition(2), "-"),
                           Token(TokenType::Star, MakePosition(2), "*"),
                           Token(TokenType::Slash, MakePosition(4), "/"),
                           Token(TokenType::EndOfFile, MakePosition(4), "") });
}

TEST(ScannerTest, BinaryExpression)
{
    GenericReadAllTokens("3 + \t2.4 * 5 -\n (4.0 / 2.1)",
                         { Token(TokenType::Number, MakePosition(1), "3"),
                           Token(TokenType::Plus, MakePosition(1), "+"),
                           Token(TokenType::Number, MakePosition(1), "2.4"),
                           Token(TokenType::Star, MakePosition(1), "*"),
                           Token(TokenType::Number, MakePosition(1), "5"),
                           Token(TokenType::Minus, MakePosition(1), "-"),
                           Token(TokenType::LeftParenthesis, MakePosition(2), "("),
                           Token(TokenType::Number, MakePosition(2), "4.0"),
                           Token(TokenType::Slash, MakePosition(2), "/"),
                           Token(TokenType::Number, MakePosition(2), "2.1"),
                           Token(TokenType::RightParenthesis, MakePosition(2), ")"),
                           Token(TokenType::EndOfFile, MakePosition(2), ""),
                         });
}

TEST(ScannerTest, Number)
{
    GenericFirstTokenCheck("3.14", TokenType::Number);
}

TEST(ScannerTest, Plus)
{
    GenericFirstTokenCheck("+", TokenType::Plus);
}

TEST(ScannerTest, Minus)
{
    GenericFirstTokenCheck("-", TokenType::Minus);
}

TEST(ScannerTest, Star)
{
    GenericFirstTokenCheck("*", TokenType::Star);
}

TEST(ScannerTest, Slash)
{
    GenericFirstTokenCheck("/", TokenType::Slash);
}

TEST(ScannerTest, LeftParenthesis)
{
    GenericFirstTokenCheck("(", TokenType::LeftParenthesis);
}

TEST(ScannerTest, RightParenthesis)
{
    GenericFirstTokenCheck(")", TokenType::RightParenthesis);
}

TEST(ScannerTest, Identifier)
{
    GenericFirstTokenCheck("abcABC_123_", TokenType::Identifier);
}

TEST(ScannerTest, TrueBool)
{
    GenericFirstTokenCheck("true", TokenType::True);
}

TEST(ScannerTest, FalseBool)
{
    GenericFirstTokenCheck("false", TokenType::False);
}

TEST(ScannerTest, NilKeyword)
{
    GenericFirstTokenCheck("nil", TokenType::Nil);
}

TEST(ScannerTest, Equal)
{
    GenericFirstTokenCheck("=", TokenType::Equal);
}

TEST(ScannerTest, EqualEqual)
{
    GenericFirstTokenCheck("==", TokenType::EqualEqual);
}

TEST(ScannerTest, EqualWhitespaceEqual)
{
    GenericReadAllTokens("= =", { Token(TokenType::Equal, MakePosition(1), "="),
                                  Token(TokenType::Equal, MakePosition(1), "="),
                                  Token(TokenType::EndOfFile, MakePosition(1), "") });
}

TEST(ScannerTest, Greater)
{
    GenericFirstTokenCheck(">", TokenType::Greater);
}

TEST(ScannerTest, GreaterEqual)
{
    GenericFirstTokenCheck(">=", TokenType::GreaterEqual);
}

TEST(ScannerTest, Less)
{
    GenericFirstTokenCheck("<", TokenType::Less);
}

TEST(ScannerTest, LessEqual)
{
    GenericFirstTokenCheck("<=", TokenType::LessEqual);
}

TEST(ScannerTest, Bang)
{
    GenericFirstTokenCheck("!", TokenType::Bang);
}

TEST(ScannerTest, BangEqual)
{
    GenericFirstTokenCheck("!=", TokenType::BangEqual);
}

TEST(ScannerTest, String)
{
    GenericFirstTokenCheck("\"str\"", TokenType::String);
}

TEST(ScannerTest, UnterminatedString)
{
    GenericFirstTokenCheck("\"str", TokenType::Error);
}

TEST(ScannerTest, Semicolon)
{
    GenericFirstTokenCheck(";", TokenType::Semicolon);
}

TEST(ScannerTest, Print)
{
    GenericFirstTokenCheck("print", TokenType::Print);
}

TEST(ScannerTest, Var)
{
    GenericFirstTokenCheck("var", TokenType::Var);
}

TEST(ScannerTest, OneLineComment)
{
    GenericFirstTokenCheck("// Hello, comment!\nabc", TokenType::Identifier);
}

TEST(ScannerTest, LeftBracket)
{
    GenericFirstTokenCheck("{", TokenType::LeftBracket);
}

TEST(ScannerTest, RightBracket)
{
    GenericFirstTokenCheck("}", TokenType::RightBracket);
}

TEST(ScannerTest, If)
{
    GenericFirstTokenCheck("if", TokenType::If);
}

TEST(ScannerTest, Else)
{
    GenericFirstTokenCheck("else", TokenType::Else);
}

TEST(ScannerTest, And)
{
    GenericFirstTokenCheck("and", TokenType::And);
}

TEST(ScannerTest, Or)
{
    GenericFirstTokenCheck("or", TokenType::Or);
}

TEST(ScannerTest, While)
{
    GenericFirstTokenCheck("while", TokenType::While);
}

TEST(ScannerTest, Break)
{
    GenericFirstTokenCheck("break", TokenType::Break);
}

TEST(ScannerTest, Continue)
{
    GenericFirstTokenCheck("continue", TokenType::Continue);
}

TEST(ScannerTest, Fun)
{
    GenericFirstTokenCheck("fun", TokenType::Fun);
}

TEST(ScannerTest, Comma)
{
    GenericFirstTokenCheck(",", TokenType::Comma);
}

TEST(ScannerTest, Return)
{
    GenericFirstTokenCheck("return", TokenType::Return);
}

TEST(ScannerTest, Class)
{
    GenericFirstTokenCheck("class", TokenType::Class);
}

void GenericFirstTokenCheck(std::string_view input, TokenType actualType)
{
    Scanner scanner("no-file", input);
    Token token = scanner++;
    
    EXPECT_EQ(token.GetType(), actualType);
}

void GenericReadAllTokens(std::string_view input, std::initializer_list<Token> tokens)
{
    Scanner scanner("no-file", input);
    
    for (auto& needToken: tokens)
    {
        Token token = scanner++;
        
        EXPECT_EQ(needToken.GetType(), token.GetType());
        EXPECT_EQ(needToken.GetPos(), token.GetPos());
        EXPECT_EQ(needToken.GetStr(), token.GetStr());
    }
}

SourcePosition MakePosition(std::size_t line)
{
    return SourcePosition(line);
}
