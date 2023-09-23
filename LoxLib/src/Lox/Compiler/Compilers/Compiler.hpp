#ifndef LOX_VM_COMPILER_BASE_COMPILER_HPP
#define LOX_VM_COMPILER_BASE_COMPILER_HPP

#include <string_view>
#include <optional>

#include "Lox/Interpreter/VirtualMachine.hpp"

#include "Lox/Runtime/Objects/Closure.hpp"

#include "Lox/Compiler/CompilerErrorReporter.hpp"

#include "../Scanning/Scanner.hpp"
#include "../Parsing/Parser.hpp"
#include "../Parsing/Precedence.hpp"
#include "../Parsing/ParserRule.hpp"

#include "Lox/Util/StackedBoolean.hpp"
#include "Lox/Util/OptionalReference.hpp"

#include "ScopedValueChange.hpp"
#include "FunctionType.hpp"
#include "ClassCompiler.hpp"

namespace Lox
{
    class Compiler
    {
    public:
        Compiler(VirtualMachine& vm, OptionalReference<Compiler> parent, std::optional<ClassCompiler> currentClass,
                 Parser& parser, FunctionType type, Token name);
        
        GcPtr<Closure> Compile();
    
    protected:
        // Main state.
        
        VirtualMachine& vm;
        OptionalReference<Compiler> parent;
        Parser& parser;
        GcPtr<Closure> function;
        FunctionType type;
        std::optional<ClassCompiler> currentClass;
        
        // Main functions.
        
        GcPtr<Closure> EndCompiling();
        
        void EmitReturn();
        
        void CompileScript();
        
        void CompileFunction();
        
        // Declarations.
        
        void Declaration();
        
        void VariableDeclaration();
        
        void FunctionDeclaration();
        
        void Function(Token name, FunctionType funcType);
        
        size_t ParametersList();
        
        void ClassDeclaration();
        
        void Method();
        
        // Statements.
        
        void Statement();
        
        void PrintStatement();
        
        void BlockStatement();
        
        void IfStatement();
        
        void WhileStatement();
        
        void BreakStatement();
        
        void ContinueStatement();
        
        void LoopJumpStatementHelper(Opcode opcode, std::vector<std::size_t>& vector);
        
        void ReturnStatement();
        
        void ExpressionStatement();
        
        // Expressions.
        
        void Expression();
        
        void Binary(bool canAssign);
        
        void Unary(bool canAssign);
        
        void And(bool canAssign);
        
        void Or(bool canAssign);
        
        void Dot(bool canAssign);
        
        void Grouping(bool canAssign);
        
        void Variable(bool canAssign);
        
        void DoubleNumber(bool canAssign);
        
        void Literal(bool canAssign);
        
        void This(bool canAssign);
        
        void Super(bool canAssign);
        
        void Call(bool canAssign);
        
        std::size_t ArgumentsList();
        
        // Expressions machinery.
        
        void ParsePrecedence(Precedence prec);
        
        void CallParseFn(ParseFn fn, bool canAssign);
        
        // I hoped to separate the expression compiler from statements compiler, but
        // it is impossible, because the expression accesses the locals.
        
        static std::unordered_map<unsigned, ParserRule> rulesMap;
        
        static ParserRule GetParserRule(TokenType type);
        
        static Opcode TokenTypeToBasicBinaryOperation(TokenType type);
        
        // Variables.
        
        using Local = std::tuple<Token, std::size_t, bool>;
        
        std::vector<Local> locals;
        std::size_t scopeLevel;
        
        void BeginScope();
        
        void EndScope();
        
        /// Return stack offset for the local if it exists,
        /// otherwise return -1.
        int ResolveLocal(std::string_view name);
        
        void CleanUpLocals(std::size_t toScopeLevel);
        
        void DefineVariable(Token name);
        
        void DefineGlobalVariable(Token name);
        
        void DefineLocalVariable(Token name);
        
        uint8_t VariableName(Token name);
        
        void NamedVariable(Token name, bool canAssign);
        
        // Upvalues.
        
        using UpvalueInfo = std::tuple<std::size_t, bool>;
        
        int ResolveUpvalue(std::string_view name);
        
        std::size_t AddUpvalue(std::size_t arg, bool isLocal);
        
        std::vector<UpvalueInfo> upvalues;
        
        // Loop stuff.
        
        std::optional<std::size_t> currentLoopScope;
        
        std::vector<std::size_t> breaks;
        std::vector<std::size_t> continues;
        
        void PatchBreaks();
        
        void PatchContinues(std::size_t loopStart);
        
        // Emitter.
        
        void EmitString(std::string_view view);
        
        /// Add constant to chunk and add `Opcode::PushConstant` to the code.
        void EmitConstant(Value val, SourcePosition pos);
        
        void EmitConstant(Value val);
        
        std::size_t EmitJump(Opcode jump);
        
        void PatchJumpWith(std::size_t index, std::size_t offset);
        
        void PatchJump(std::size_t index);
        
        void EmitLoop(std::size_t to);
        
        void PatchLoopWith(std::size_t index, std::size_t val);
        
        /// Add constant to chunk.
        uint8_t AddConstant(Value val, SourcePosition pos);
        
        uint8_t AddConstant(Value val);
        
        void EmitByte(uint8_t byte, SourcePosition pos);
        
        void EmitByte(uint8_t byte);
        
        void EmitOpcode(Opcode opcode, SourcePosition pos);
        
        void EmitOpcode(Opcode opcode);
        
        // Util.
        
        OptionalReference<Compiler> ThisReference();
        
        Chunk& GetCurrentChunk();
        
        void Synchronize();
        
        void SaveFirstLocal();
    }; // class Compiler
}

#endif // LOX_VM_COMPILER_BASE_COMPILER_HPP
