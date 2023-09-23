#include "Compiler.hpp"

#include <charconv>
#include <cstdint>
#include <exception>
#include <ranges>
#include <utility>

#include "Lox/Interpreter/Opcode.hpp"

#include "Lox/Util/Assert.hpp"
#include "Lox/Util/ChunkDumper.hpp"

#include "Lox/Runtime/Objects/String.hpp"

namespace Lox
{
    Compiler::Compiler(VirtualMachine& vm, OptionalReference<Compiler> parent,
                       std::optional<ClassCompiler> currentClass,
                       Parser& parser, FunctionType type, Token name)
            : vm(vm), parent(std::move(parent)), parser(parser),
              function(vm.AllocateObject<Closure>(vm.InternString(name.GetStr()), 0, Chunk())),
              type(type),
              scopeLevel(0),
              currentClass(std::move(currentClass))
    {
        SaveFirstLocal();
    }
    
    // Main functions.
    
    GcPtr<Closure> Compiler::Compile()
    {
        switch (type)
        {
        case FunctionType::Script:
            CompileScript();
            break;
        case FunctionType::Default:
        case FunctionType::Method:
        case FunctionType::Initializer:
            CompileFunction();
            break;
        }
        
        return EndCompiling();
    }
    
    GcPtr<Closure> Compiler::EndCompiling()
    {
        EmitReturn();
        
        if constexpr (Configuration::DumpChunkAfterCompile)
        {
            if (!parser.HadError())
            {
                ChunkDumper dumper(function->GetChunk(), function->GetName()->GetCppString(), std::cout);
                dumper.Dump();
            }
        }
        
        return parser.HadError() ? GcPtr<Closure>(nullptr) : function;
    }
    
    void Compiler::EmitReturn()
    {
        if (type == FunctionType::Initializer)
        {
            EmitOpcode(OpcodeGetLocal);
            EmitByte(0);
        }
        else
        {
            EmitOpcode(OpcodeNil);
        }
        
        EmitOpcode(OpcodeReturn);
    }
    
    void Compiler::CompileScript()
    {
        parser.Advance();
        
        while (!parser.IsAtEnd())
        {
            try
            {
                Declaration();
            }
            catch (const ParserException& e)
            {
                Synchronize();
            }
        }
    }
    
    void Compiler::CompileFunction()
    {
        BeginScope(); // NOTE: No end scope.
        
        parser.Require(TokenType::LeftParenthesis, "expected '(' after function name");
        std::size_t arity = ParametersList();
        function->UglyChangeArity(arity);
        parser.Require(TokenType::RightParenthesis, "expected ')' after function parameters");
        
        parser.Require(TokenType::LeftBracket, "expected '{' after function prototype");
        
        try
        {
            BlockStatement();
        }
        catch (const ParserException& e)
        {
            Synchronize();
        }
    }
    
    std::size_t Compiler::ParametersList()
    {
        std::size_t count = 0;
        
        while (!parser.CheckNext(TokenType::RightParenthesis))
        {
            do
            {
                Token name = parser.Require(TokenType::Identifier, "expected parameter name");
                DefineLocalVariable(name);
                ++count;
            } while (parser.Match(TokenType::Comma));
        }
        
        return count;
    }
    
    // Declarations.
    
    void Compiler::Declaration()
    {
        if (parser.Match(TokenType::Var))
        {
            return VariableDeclaration();
        }
        else if (parser.Match(TokenType::Fun))
        {
            return FunctionDeclaration();
        }
        else if (parser.Match(TokenType::Class))
        {
            return ClassDeclaration();
        }
        else
        {
            return Statement();
        }
    }
    
    void Compiler::FunctionDeclaration()
    {
        Token name = parser.Require(TokenType::Identifier, "expected function name");
        Function(name, FunctionType::Default);
        DefineVariable(name);
    }
    
    void Compiler::Function(Token name, FunctionType funcType)
    {
        Compiler compiler(vm, ThisReference(), currentClass, parser, funcType, name);
        GcPtr<Closure> func = compiler.Compile();
        
        EmitConstant(Value(func), name.GetPos());
        
        const std::vector<UpvalueInfo>& funcUpvalues = compiler.upvalues;
        
        if (!funcUpvalues.empty())
        {
            EmitOpcode(OpcodeFillUpvalues);
            EmitByte(funcUpvalues.size());
            
            for (const auto& upvalue: funcUpvalues)
            {
                EmitByte(std::get<bool>(upvalue));
                EmitByte(std::get<std::size_t>(upvalue));
            }
        }
    }
    
    void Compiler::ClassDeclaration()
    {
        ScopedValueChange<std::optional<ClassCompiler>> newCompiler(ClassCompiler(), currentClass);
        
        Token name = parser.Require(TokenType::Identifier, "expected function name");
        
        EmitOpcode(OpcodeClass);
        EmitByte(VariableName(name));
        DefineVariable(name);
        
        if (parser.Match(TokenType::Less))
        {
            currentClass->SetSuper(true);
            
            Token superName = parser.Require(TokenType::Identifier, "expected super class name");
            
            BeginScope();
            locals.emplace_back(Token("super"), scopeLevel, false);
            NamedVariable(superName, false);
            
            NamedVariable(name, false);
            
            EmitOpcode(OpcodeInherit);
        }
        
        NamedVariable(name, false);
        
        parser.Require(TokenType::LeftBracket, "expected '{' after class name");
        
        while (!parser.IsAtEnd() && !parser.CheckNext(TokenType::RightBracket))
        {
            Method();
        }
        
        parser.Require(TokenType::RightBracket, "expected '}' after class body");
        
        EmitOpcode(OpcodePop);
        
        if (currentClass->HasSuper())
        {
            EndScope();
        }
    }
    
    void Compiler::Method()
    {
        Token name = parser.Require(TokenType::Identifier, "expected method name");
        
        FunctionType methodType = FunctionType::Method;
        
        if (name.GetStr() == "init")
        {
            methodType = FunctionType::Initializer;
        }
        
        Function(name, methodType);
        
        EmitOpcode(OpcodeMethod);
        EmitByte(VariableName(name));
    }
    
    void Compiler::VariableDeclaration()
    {
        parser.Require(TokenType::Identifier, "expected variable name");
        Token name = parser.GetCurrent();
        
        if (parser.Match(TokenType::Equal))
        {
            Expression();
        }
        else
        {
            EmitOpcode(OpcodeNil);
        }
        
        parser.Require(TokenType::Semicolon, "expected ';' after variable declaration");
        
        DefineVariable(name);
    }
    
    // Statements.
    
    void Compiler::Statement()
    {
        if (parser.Match(TokenType::Print))
        {
            PrintStatement();
        }
        else if (parser.Match(TokenType::LeftBracket))
        {
            BeginScope();
            BlockStatement();
            EndScope();
        }
        else if (parser.Match(TokenType::If))
        {
            IfStatement();
        }
        else if (parser.Match(TokenType::While))
        {
            WhileStatement();
        }
        else if (parser.Match(TokenType::Break))
        {
            BreakStatement();
        }
        else if (parser.Match(TokenType::Continue))
        {
            ContinueStatement();
        }
        else if (parser.Match(TokenType::Return))
        {
            ReturnStatement();
        }
        else
        {
            ExpressionStatement();
        }
    }
    
    void Compiler::PrintStatement()
    {
        Expression();
        EmitOpcode(OpcodePrint);
        parser.Require(TokenType::Semicolon, "expected ';' after print statement");
    }
    
    void Compiler::BlockStatement()
    {
        while (!parser.IsAtEnd() && !parser.CheckNext(TokenType::RightBracket))
        {
            Declaration();
        }
        
        parser.Require(TokenType::RightBracket, "expected '}' after block statement");
    }
    
    void Compiler::IfStatement()
    {
        parser.Require(TokenType::LeftParenthesis, "expected '(' after if keyword");
        Expression();
        parser.Require(TokenType::RightParenthesis, "expected ')' after if condition");
        std::size_t elseEnterJump = EmitJump(OpcodeJumpIfFalse);
        
        EmitOpcode(OpcodePop);
        Statement();
        std::size_t thenExitJump = EmitJump(OpcodeJump);
        
        PatchJump(elseEnterJump);
        EmitOpcode(OpcodePop);
        if (parser.Match(TokenType::Else))
        {
            Statement();
        }
        
        PatchJump(thenExitJump);
    }
    
    void Compiler::WhileStatement()
    {
        auto oldLoopScope = currentLoopScope;
        currentLoopScope = scopeLevel;
        
        parser.Require(TokenType::LeftParenthesis, "expected '(' after while keyword");
        std::size_t conditionStart = GetCurrentChunk().GetCodeSize();
        Expression();
        std::size_t exit = EmitJump(OpcodeJumpIfFalse);
        parser.Require(TokenType::RightParenthesis, "expected ')' after while condition");
        
        EmitOpcode(OpcodePop);
        Statement();
        EmitLoop(conditionStart);
        
        PatchJump(exit);
        EmitOpcode(OpcodePop);
        
        PatchBreaks();
        PatchContinues(conditionStart);
        
        currentLoopScope = oldLoopScope;
    }
    
    void Compiler::BreakStatement()
    {
        LoopJumpStatementHelper(OpcodeJump, breaks);
    }
    
    void Compiler::ContinueStatement()
    {
        LoopJumpStatementHelper(OpcodeLoop, continues);
    }
    
    void Compiler::LoopJumpStatementHelper(Opcode opcode, std::vector<std::size_t>& vector)
    {
        if (!currentLoopScope.has_value())
        {
            parser.ErrorAtCurrent("loop jump statement occurred outside of loop");
        }
        
        CleanUpLocals(currentLoopScope.value());
        
        std::size_t jump = EmitJump(opcode);
        vector.push_back(jump);
        
        parser.Require(TokenType::Semicolon, "expected ';' after loop jump statement");
    }
    
    void Compiler::ReturnStatement()
    {
        if (type == FunctionType::Script)
        {
            parser.ErrorAtCurrent("return is not allowed there");
        }
        
        if (!parser.Match(TokenType::Semicolon))
        {
            if (type == FunctionType::Initializer)
            {
                parser.ErrorAtCurrent("returning expressions is not allowed in initializers");
            }
            
            Expression();
            parser.Require(TokenType::Semicolon, "expected ';' after return statement");
            
            EmitOpcode(OpcodeReturn);
        }
        else
        {
            EmitReturn();
        }
    }
    
    void Compiler::ExpressionStatement()
    {
        Expression();
        EmitOpcode(OpcodePop);
        parser.Require(TokenType::Semicolon, "expected ';' after expression statement");
    }
    
    // Expressions.
    
    void Compiler::Expression()
    {
        ParsePrecedence(PrecedenceOr);
    }
    
    void Compiler::And(bool canAssign)
    {
        std::size_t falseJump = EmitJump(OpcodeJumpIfFalse);
        
        EmitOpcode(OpcodePop);
        ParsePrecedence(PrecedenceAnd);
        
        PatchJump(falseJump);
    }
    
    void Compiler::Or(bool canAssign)
    {
        std::size_t jumpFalse = EmitJump(OpcodeJumpIfFalse);
        std::size_t jumpTrue = EmitJump(OpcodeJump);
        
        PatchJump(jumpFalse);
        ParsePrecedence(PrecedenceOr);
        
        PatchJump(jumpTrue);
    }
    
    void Compiler::Binary(bool canAssign)
    {
        Token op = parser.GetCurrent();
        
        ParsePrecedence(static_cast<Precedence>(GetParserRule(op.GetType()).GetPrecedence() + 1));
        
        switch (op.GetType())
        {
        case TokenType::BangEqual:
            EmitOpcode(OpcodeEqual, op.GetPos());
            EmitOpcode(OpcodeNot, op.GetPos());
            break;
        case TokenType::GreaterEqual:
            EmitOpcode(OpcodeLess, op.GetPos());
            EmitOpcode(OpcodeNot, op.GetPos());
            break;
        case TokenType::LessEqual:
            EmitOpcode(OpcodeGreater, op.GetPos());
            EmitOpcode(OpcodeNot, op.GetPos());
            break;
        default:
            EmitOpcode(TokenTypeToBasicBinaryOperation(op.GetType()), op.GetPos());
            break;
        }
    }
    
    Opcode Compiler::TokenTypeToBasicBinaryOperation(TokenType type)
    {
        switch (type)
        {
        case TokenType::Plus:
            return OpcodeAdd;
        case TokenType::Minus:
            return OpcodeSubstract;
        case TokenType::Star:
            return OpcodeMultiply;
        case TokenType::Slash:
            return OpcodeDivide;
        case TokenType::EqualEqual:
            return OpcodeEqual;
        case TokenType::Greater:
            return OpcodeGreater;
        case TokenType::Less:
            return OpcodeLess;
        default:
            LOX_UNREACHABLE("TokenTypeToBasicBinaryOperation passed wrong token type");
            return OpcodeAdd;
        }
    }
    
    void Compiler::Unary(bool canAssign)
    {
        Token op = parser.GetCurrent();
        
        ParsePrecedence(PrecedenceUnary);
        
        switch (op.GetType())
        {
        case TokenType::Plus:
            break;
        case TokenType::Minus:
            EmitOpcode(OpcodeNegate, op.GetPos());
            break;
        case TokenType::Bang:
            EmitOpcode(OpcodeNot, op.GetPos());
            break;
        default:
            LOX_UNREACHABLE("invalid token type in unary");
            break;
        }
    }
    
    void Compiler::Variable(bool canAssign)
    {
        NamedVariable(parser.GetCurrent(), canAssign);
    }
    
    void Compiler::This(bool canAssign)
    {
        Variable(false);
    }
    
    void Compiler::Super(bool canAssign)
    {
        if (!currentClass)
        {
            parser.ErrorAtCurrent("super is not allowed outside of class");
        }
        
        if (!currentClass->HasSuper())
        {
            parser.ErrorAtCurrent("current class does not have a superclass");
        }
        
        parser.Require(TokenType::Dot, "expected '.' after super");
        
        Token name = parser.Require(TokenType::Identifier, "expected method name after super");
        
        NamedVariable(Token("this"), false);
        
        if (parser.Match(TokenType::LeftParenthesis))
        {
            std::size_t argCount = ArgumentsList();
            parser.Require(TokenType::RightParenthesis, "expected ')' after arguments list");
            NamedVariable(Token("super"), false);
            EmitOpcode(OpcodeInvokeSuper);
            EmitByte(argCount);
            EmitByte(VariableName(name));
        }
        else
        {
            NamedVariable(Token("super"), false);
            EmitOpcode(OpcodeGetSuper);
            EmitByte(VariableName(name));
        }
    }
    
    void Compiler::Call(bool canAssign)
    {
        std::size_t count = ArgumentsList();
        parser.Require(TokenType::RightParenthesis, "expected ')' after arguments list");
        
        if (count > UINT8_MAX)
        {
            parser.ErrorAtCurrent("too many arguments");
        }
        
        EmitOpcode(OpcodeCall);
        EmitByte(count);
    }
    
    std::size_t Compiler::ArgumentsList()
    {
        if (parser.CheckNext(TokenType::RightParenthesis))
        {
            return 0;
        }
        
        std::size_t count = 0;
        
        do
        {
            ++count;
            Expression();
        } while (parser.Match(TokenType::Comma));
        
        return count;
    }
    
    void Compiler::Dot(bool canAssign)
    {
        Token name = parser.Require(TokenType::Identifier, "expected property name");
        uint8_t nameConstant = VariableName(name); // It's not a variable, just a string constant.
        
        if (canAssign && parser.Match(TokenType::Equal))
        {
            Expression();
            EmitOpcode(OpcodeSetProperty);
        }
        else if (parser.Match(TokenType::LeftParenthesis))
        {
            std::size_t argCount = ArgumentsList();
            parser.Require(TokenType::RightParenthesis, "expected ')' after arguments list");
            
            EmitOpcode(OpcodeInvoke);
            EmitByte(argCount);
        }
        else
        {
            EmitOpcode(OpcodeGetProperty);
        }
        
        EmitByte(nameConstant);
    }
    
    void Compiler::Grouping(bool canAssign)
    {
        Expression();
        parser.Require(TokenType::RightParenthesis, "expected ')'");
    }
    
    void Compiler::DoubleNumber(bool canAssign)
    {
        double number;
        std::from_chars_result res =
                std::from_chars(parser.GetCurrent().GetStr().begin(),
                                parser.GetCurrent().GetStr().end(),
                                number);
        
        if (res.ec == std::errc::result_out_of_range)
        {
            parser.ErrorAtCurrent("number is out of allowed range");
        }
        else if (res.ec == std::errc::invalid_argument)
        {
            LOX_UNREACHABLE("Lox::Compiler::Scanner does not recognize double numbers correctly");
        }
        
        EmitConstant(Value(number));
    }
    
    void Compiler::Literal(bool canAssign)
    {
        switch (parser.GetCurrent().GetType())
        {
        case TokenType::Nil:
            EmitOpcode(OpcodeNil);
            break;
        case TokenType::True:
            EmitOpcode(OpcodeTrue);
            break;
        case TokenType::False:
            EmitOpcode(OpcodeFalse);
            break;
        case TokenType::String:
            EmitString(parser.GetCurrent().GetStr());
            break;
        default:
            LOX_UNREACHABLE("invalid token type for literal");
            break;
        }
    }
    
    // Expressions machinery.
    
    void Compiler::ParsePrecedence(Precedence prec)
    {
        bool canAssign = prec <= PrecedenceAssignment;
        
        parser.Advance();
        
        ParseFn prefix = GetParserRule(parser.GetCurrent().GetType()).GetPrefix();
        if (!prefix)
        {
            parser.ErrorAtCurrent("expected expression");
        }
        
        CallParseFn(prefix, canAssign);
        
        while (GetParserRule(parser.GetNext().GetType()).GetPrecedence() >= prec)
        {
            parser.Advance();
            
            ParseFn infix = GetParserRule(parser.GetCurrent().GetType()).GetInfix();
            if (!infix)
            {
                parser.ErrorAtCurrent("expected expression");
            }
            
            CallParseFn(infix, canAssign);
        }
    }
    
    void Compiler::CallParseFn(ParseFn fn, bool canAssign)
    {
        (this->*fn)(canAssign);
    }
    
    ParserRule Compiler::GetParserRule(TokenType type)
    {
        auto it = rulesMap.find(static_cast<unsigned>(type));
        
        if (it != rulesMap.end())
        {
            return it->second;
        }
        else
        {
            return ParserRule();
        }
    }
    
    std::unordered_map<unsigned, ParserRule> Compiler::rulesMap =
            {
                    {
                            static_cast<unsigned>(TokenType::Number),
                            ParserRule(&Compiler::DoubleNumber, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::Plus),
                            ParserRule(&Compiler::Unary, &Compiler::Binary, PrecedenceTerm)
                    },
                    {
                            static_cast<unsigned>(TokenType::Minus),
                            ParserRule(&Compiler::Unary, &Compiler::Binary, PrecedenceTerm)
                    },
                    {
                            static_cast<unsigned>(TokenType::Star),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceFactor)
                    },
                    {
                            static_cast<unsigned>(TokenType::Slash),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceFactor)
                    },
                    {
                            static_cast<unsigned>(TokenType::Bang),
                            ParserRule(&Compiler::Unary, nullptr, PrecedenceUnary)
                    },
                    {
                            static_cast<unsigned>(TokenType::LeftParenthesis),
                            ParserRule(&Compiler::Grouping, &Compiler::Call, PrecedenceCall)
                    },
                    {
                            static_cast<unsigned>(TokenType::Nil),
                            ParserRule(&Compiler::Literal, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::True),
                            ParserRule(&Compiler::Literal, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::False),
                            ParserRule(&Compiler::Literal, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::Identifier),
                            ParserRule(&Compiler::Variable, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::Greater),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceComparison)
                    },
                    {
                            static_cast<unsigned>(TokenType::Less),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceComparison)
                    },
                    {
                            static_cast<unsigned>(TokenType::Equal),
                            ParserRule(nullptr, nullptr, PrecedenceAssignment)
                    },
                    {
                            static_cast<unsigned>(TokenType::BangEqual),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceEquality)
                    },
                    {
                            static_cast<unsigned>(TokenType::EqualEqual),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceEquality)
                    },
                    {
                            static_cast<unsigned>(TokenType::GreaterEqual),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceComparison)
                    },
                    {
                            static_cast<unsigned>(TokenType::LessEqual),
                            ParserRule(nullptr, &Compiler::Binary, PrecedenceComparison)
                    },
                    {
                            static_cast<unsigned>(TokenType::String),
                            ParserRule(&Compiler::Literal, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::And),
                            ParserRule(nullptr, &Compiler::And, PrecedenceAnd)
                    },
                    {
                            static_cast<unsigned>(TokenType::Or),
                            ParserRule(nullptr, &Compiler::Or, PrecedenceOr)
                    },
                    {
                            static_cast<unsigned>(TokenType::Dot),
                            ParserRule(nullptr, &Compiler::Dot, PrecedenceCall)
                    },
                    {
                            static_cast<unsigned>(TokenType::This),
                            ParserRule(&Compiler::This, nullptr, PrecedencePrimary)
                    },
                    {
                            static_cast<unsigned>(TokenType::Super),
                            ParserRule(&Compiler::Super, nullptr, PrecedencePrimary)
                    }
            };
    
    // Loop stuff.
    
    void Compiler::PatchBreaks()
    {
        LOX_ASSERT(currentLoopScope.has_value(), "patching breaks outside of loop");
        
        for (auto brk: breaks)
        {
            PatchJump(brk);
        }
        breaks.clear();
    }
    
    void Compiler::PatchContinues(std::size_t loopStart)
    {
        LOX_ASSERT(currentLoopScope.has_value(), "patching continues outside of loop");
        
        for (auto cont: continues)
        {
            PatchLoopWith(cont, loopStart);
        }
        continues.clear();
    }
    
    // Variables.
    
    void Compiler::DefineVariable(Token name)
    {
        if (scopeLevel == 0)
        {
            DefineGlobalVariable(name);
        }
        else
        {
            DefineLocalVariable(name);
        }
    }
    
    void Compiler::DefineGlobalVariable(Token name)
    {
        SourcePosition pos = name.GetPos();
        EmitOpcode(OpcodeDefineGlobal, pos);
        EmitByte(VariableName(name), pos);
    }
    
    void Compiler::DefineLocalVariable(Token name)
    {
        for (auto it = locals.rbegin(); it != locals.rend() && std::get<std::size_t>(*it) == scopeLevel; ++it)
        {
            if (name.GetStr() == std::get<Token>(*it).GetStr())
            {
                parser.ErrorAtCurrent("redefinition of local variable");
            }
        }
        
        if (locals.size() > UINT8_MAX)
        {
            parser.ErrorAtCurrent("too many captured variables in function");
        }
        
        locals.emplace_back(name, scopeLevel, false);
    }
    
    uint8_t Compiler::VariableName(Token name)
    {
        GcPtr<String> nameString = vm.InternString(name.GetStr());
        Value nameValue(nameString);
        return AddConstant(nameValue, name.GetPos());
    }
    
    void Compiler::NamedVariable(Token name, bool canAssign)
    {
        Opcode getOp;
        Opcode setOp;
        
        int arg = ResolveLocal(name.GetStr());
        
        if (arg != -1)
        {
            getOp = OpcodeGetLocal;
            setOp = OpcodeSetLocal;
        }
        else if ((arg = ResolveUpvalue(name.GetStr())) != -1)
        {
            getOp = OpcodeGetUpvalue;
            setOp = OpcodeSetUpvalue;
        }
        else
        {
            arg = VariableName(name.GetStr());
            getOp = OpcodeGetGlobal;
            setOp = OpcodeSetGlobal;
        }
        
        if (parser.Match(TokenType::Equal))
        {
            if (!canAssign)
            {
                parser.ErrorAtCurrent("invalid assignment target");
            }
            
            Expression();
            EmitOpcode(setOp);
            EmitByte(arg);
        }
        else
        {
            EmitOpcode(getOp);
            EmitByte(arg);
        }
    }
    
    void Compiler::CleanUpLocals(std::size_t toScopeLevel)
    {
        for (auto local = locals.rbegin(); local != locals.rend(); ++local)
        {
            if (std::get<size_t>(*local) <= toScopeLevel)
            {
                return;
            }
            
            EmitOpcode(OpcodePop);
        }
    }
    
    void Compiler::BeginScope()
    {
        scopeLevel++;
    }
    
    void Compiler::EndScope()
    {
        scopeLevel--;
        
        while (!locals.empty()
               && std::get<std::size_t>(locals.back()) > scopeLevel)
        {
            if (std::get<bool>(locals.back()))
            {
                EmitOpcode(OpcodeCloseUpvalue);
            }
            else
            {
                EmitOpcode(OpcodePop);
            }
            
            locals.pop_back();
        }
    }
    
    int Compiler::ResolveLocal(std::string_view name)
    {
        for (auto it = locals.rbegin(); it != locals.rend(); ++it)
        {
            if (std::get<Token>(*it).GetStr() == name)
            {
                return it.base() - locals.begin() - 1;
            }
        }
        
        return -1;
    }
    
    // Upvalues.
    
    std::size_t Compiler::AddUpvalue(std::size_t arg, bool isLocal)
    {
        UpvalueInfo newUpvalue(arg, isLocal);
        
        auto it = std::find(upvalues.begin(), upvalues.end(), newUpvalue);
        
        if (it != upvalues.end())
        {
            return it - upvalues.begin();
        }
        
        upvalues.push_back(newUpvalue);
        
        if (isLocal)
        {
            std::get<bool>(parent->get().locals[arg]) = true;
        }
        
        return upvalues.size() - 1;
    }
    
    int Compiler::ResolveUpvalue(std::string_view name)
    {
        if (!parent.has_value())
        {
            return -1;
        }
        
        int local = parent->get().ResolveLocal(name);
        if (local != -1)
        {
            return AddUpvalue(local, true);
        }
        
        int upvalue = parent->get().ResolveUpvalue(name);
        if (upvalue != -1)
        {
            return AddUpvalue(upvalue, false);
        }
        
        return -1;
    }
    
    // Emitter.
    
    void Compiler::EmitString(std::string_view view)
    {
        GcPtr<String> obj = vm.InternString(std::string_view(view.begin() + 1, view.end() - 1));
        Value val(obj);
        EmitConstant(val);
    }
    
    void Compiler::EmitConstant(Value val, SourcePosition pos)
    {
        uint8_t index = AddConstant(val, pos);
        EmitOpcode(OpcodePushConstant, pos);
        EmitByte(index, pos);
    }
    
    void Compiler::EmitConstant(Value val)
    {
        EmitConstant(val, parser.GetCurrent().GetPos());
    }
    
    void Compiler::EmitByte(uint8_t byte, SourcePosition pos)
    {
        GetCurrentChunk().PushCode(byte, pos.GetLine());
    }
    
    void Compiler::EmitByte(uint8_t byte)
    {
        EmitByte(byte, parser.GetCurrent().GetPos());
    }
    
    void Compiler::EmitOpcode(Opcode opcode, SourcePosition pos)
    {
        EmitByte(opcode, pos);
    }
    
    void Compiler::EmitOpcode(Opcode opcode)
    {
        EmitOpcode(opcode, parser.GetCurrent().GetPos());
    }
    
    uint8_t Compiler::AddConstant(Lox::Value val)
    {
        return AddConstant(val, parser.GetCurrent().GetPos());
    }
    
    uint8_t Compiler::AddConstant(Lox::Value val, SourcePosition pos)
    {
        std::size_t index = GetCurrentChunk().PushConstant(val, pos.GetLine());
        
        if (index < UINT8_MAX)
        {
            return index;
        }
        else
        {
            parser.ErrorAtCurrent("too much constants");
            return 0; // To suppress warning.
        }
    }
    
    std::size_t Compiler::EmitJump(Opcode jump)
    {
        EmitByte(jump);
        EmitByte(0xFF);
        EmitByte(0xFF);
        
        return GetCurrentChunk().GetCodeSize() - 2;
    }
    
    void Compiler::PatchJumpWith(std::size_t index, std::size_t offset)
    {
        LOX_ASSERT(index < GetCurrentChunk().GetCodeSize(), "wrong index for jump patch");
        if (offset > UINT16_MAX)
        {
            parser.ErrorAtCurrent("jump block is too big");
        }
        
        GetCurrentChunk().GetCode(index) = (offset >> 8) & 0xFF;
        GetCurrentChunk().GetCode(index + 1) = offset & 0xFF;
    }
    
    void Compiler::PatchJump(std::size_t index)
    {
        std::size_t offset = GetCurrentChunk().GetCodeSize() - index - 2;
        PatchJumpWith(index, offset);
    }
    
    void Compiler::EmitLoop(std::size_t to)
    {
        std::size_t jump = EmitJump(OpcodeLoop);
        PatchLoopWith(jump, to);
    }
    
    void Compiler::PatchLoopWith(std::size_t index, std::size_t val)
    {
        std::size_t offset = index - val + 2;
        PatchJumpWith(index, offset);
    }
    
    // Util.
    
    Chunk& Compiler::GetCurrentChunk()
    {
        return function->GetChunk();
    }
    
    void Compiler::Synchronize()
    {
        while (!parser.IsAtEnd())
        {
            parser.Advance();
            
            if (parser.CheckCurrent(TokenType::Semicolon))
            {
                break;
            }
            
            switch (parser.GetNext().GetType())
            {
            case TokenType::Print:
            case TokenType::Var:
                return;
            default:
                continue;
            }
        }
    }
    
    void Compiler::SaveFirstLocal()
    {
        if (type == FunctionType::Method || type == FunctionType::Initializer)
        {
            locals.emplace_back(Token(TokenType::This, parser.GetCurrent().GetPos(), "this"), 0, false);
        }
        else
        {
            locals.emplace_back(Token(), 0, false);
        }
    }
    
    OptionalReference<Compiler> Compiler::ThisReference()
    {
        return OptionalReference<Compiler>(std::ref(*this));
    }
}
