#include <gtest/gtest.h>

#include <Lox/Lox.hpp>
#include <utility>

using namespace Lox;

class TestErrorReporter final : public CompilerErrorReporter
{
public:
    void Error(SourcePosition pos, std::string_view msg) override
    {
        std::cout << "Error [" << pos << "]: " << msg << "." << std::endl;
    }
};

static TestErrorReporter errorReporter;

void GenericTestChunkPass(std::string_view input, const std::vector<uint8_t>& bytes, std::vector<Value> constants);

void GenericTestChunkFail(std::string_view input);

void
GenericTestChunk(std::string_view input, const std::vector<uint8_t>& bytes, std::vector<Value> constants,
                 bool shouldFail);

TEST(CompilerTest, BinaryExpression)
{
    GenericTestChunkPass("(2 + 3 * 5) / 6;",
                         {
                                 OpcodePushConstant,
                                 0, // 2
                                 OpcodePushConstant,
                                 1, // 3
                                 OpcodePushConstant,
                                 2, // 5
                                 OpcodeMultiply,
                                 OpcodeAdd,
                                 OpcodePushConstant,
                                 3, // 6
                                 OpcodeDivide,
                                 OpcodePop,
                                 OpcodeNil,
                                 OpcodeReturn
                         },
                         {
                                 Value(2.0),
                                 Value(3.0),
                                 Value(5.0),
                                 Value(6.0),
                         });
}

TEST(CompilerTest, Nil)
{
    GenericTestChunkPass("nil;",
                         {
                                 OpcodeNil,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, True)
{
    GenericTestChunkPass("true;",
                         {
                                 OpcodeTrue,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, False)
{
    GenericTestChunkPass("false;",
                         {
                                 OpcodeFalse,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, Bang)
{
    GenericTestChunkPass("!true;",
                         {
                                 OpcodeTrue,
                                 OpcodeNot,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, Negate)
{
    GenericTestChunkPass("-2;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodeNegate,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(2.0) });
}

TEST(CompilerTest, BangBang)
{
    GenericTestChunkPass("!!false;",
                         {
                                 OpcodeFalse,
                                 OpcodeNot,
                                 OpcodeNot,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, EqualEqual)
{
    GenericTestChunkPass("3 == 5;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeEqual,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(3.0), Value(5.0) });
}

TEST(CompilerTest, BangEqual)
{
    GenericTestChunkPass("9 == nil;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodeNil,
                                 OpcodeEqual,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(9.0) });
}

TEST(CompilerTest, Greater)
{
    GenericTestChunkPass("2 > 1;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeGreater,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(2.0), Value(1.0) });
}

TEST(CompilerTest, GreaterEqual)
{
    GenericTestChunkPass("8 >= 5;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeLess,
                                 OpcodeNot,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(8.0), Value(5.0) });
}

TEST(CompilerTest, Less)
{
    GenericTestChunkPass("2 < 1;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeLess,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(2.0), Value(1.0) });
}

TEST(CompilerTest, LessEqual)
{
    GenericTestChunkPass("8 <= 5;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeGreater,
                                 OpcodeNot,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(8.0), Value(5.0) });
}

TEST(CompilerTest, EqualityComparison)
{
    GenericTestChunkPass("5 < 3 == 2 > 1 != false;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeLess,
                                 OpcodePushConstant,
                                 2,
                                 OpcodePushConstant,
                                 3,
                                 OpcodeGreater,
                                 OpcodeEqual,
                                 OpcodeFalse,
                                 OpcodeEqual,
                                 OpcodeNot,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(5.0),
                                 Value(3.0),
                                 Value(2.0),
                                 Value(1.0)
                         });
}

TEST(CompilerTest, EqCompBin)
{
    GenericTestChunkPass("2 - 1 > 4 + 4 == 2 * 3 < 16 / 4;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePushConstant,
                                 1,
                                 OpcodeSubstract,
                                 OpcodePushConstant,
                                 2,
                                 OpcodePushConstant,
                                 3,
                                 OpcodeAdd,
                                 OpcodeGreater,
                                 OpcodePushConstant,
                                 4,
                                 OpcodePushConstant,
                                 5,
                                 OpcodeMultiply,
                                 OpcodePushConstant,
                                 6,
                                 OpcodePushConstant,
                                 7,
                                 OpcodeDivide,
                                 OpcodeLess,
                                 OpcodeEqual,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(2.0),
                                 Value(1.0),
                                 Value(4.0),
                                 Value(4.0),
                                 Value(2.0),
                                 Value(3.0),
                                 Value(16.0),
                                 Value(4.0)
                         });
}

TEST(CompilerTest, Block)
{
    GenericTestChunkPass("{print true;}",
                         {
                                 OpcodeTrue,
                                 OpcodePrint,
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

static VirtualMachineConfiguration conf(std::cout, std::cin, std::cout);
static VirtualMachine vm(conf);

TEST(CompilerTest, CallZero)
{
    GenericTestChunkPass("func();",
                         {
                                 OpcodeGetGlobal,
                                 0,
                                 OpcodeCall,
                                 0,
                                 OpcodePop,
                                 OpcodeNil,
                                 OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("func"))
                         });
}

TEST(CompilerTest, CallOne)
{
    GenericTestChunkPass("func(true);",
                         {
                                 OpcodeGetGlobal,
                                 0,
                                 OpcodeTrue,
                                 OpcodeCall,
                                 1,
                                 OpcodePop,
                                 OpcodeNil,
                                 OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("func"))
                         });
}

TEST(CompilerTest, CallTwo)
{
    GenericTestChunkPass("func(true, false);",
                         {
                                 OpcodeGetGlobal,
                                 0,
                                 OpcodeTrue,
                                 OpcodeFalse,
                                 OpcodeCall,
                                 2,
                                 OpcodePop,
                                 OpcodeNil,
                                 OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("func"))
                         });
}

TEST(CompilerTest, CallNoRightParen)
{
    GenericTestChunkFail("func(;");
}

TEST(CompilerTest, DefineLocalVar)
{
    GenericTestChunkPass("{var a = false;}",
                         {
                                 OpcodeFalse,
                                 
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, GetLocalVar)
{
    GenericTestChunkPass("{var a; print a;}",
                         {
                                 OpcodeNil,
                                 
                                 OpcodeGetLocal,
                                 1,
                                 OpcodePrint,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, TwoLocalsSameName)
{
    GenericTestChunkFail("{var a; var a;}");
}

TEST(CompilerTest, LocalsSet)
{
    GenericTestChunkPass("{var a; a = true;}",
                         {
                                 OpcodeNil,
                                 
                                 OpcodeTrue,
                                 OpcodeSetLocal,
                                 1,
                                 OpcodePop,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, String)
{
    GenericTestChunkPass("\"str\";",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("str"))
                         });
}

TEST(CompilerTest, PrintStmt)
{
    GenericTestChunkPass("print 42;",
                         {
                                 OpcodePushConstant,
                                 0,
                                 OpcodePrint,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(42.0)
                         });
}

TEST(CompilerTest, VarDecl)
{
    GenericTestChunkPass("var a = true;",
                         {
                                 OpcodeTrue,
                                 OpcodeDefineGlobal,
                                 0,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("a"))
                         });
}

TEST(CompilerTest, VarDeclNil)
{
    GenericTestChunkPass("var b;",
                         {
                                 OpcodeNil,
                                 OpcodeDefineGlobal,
                                 0,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("b"))
                         });
}

TEST(CompilerTest, GlobalVarGet)
{
    GenericTestChunkPass("abc;",
                         {
                                 OpcodeGetGlobal,
                                 0,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         { Value(vm.InternString("abc")) });
}

TEST(CompilerTest, GlobalVarSet)
{
    GenericTestChunkPass("abc = 123;",
                         {
                                 OpcodePushConstant,
                                 1,
                                 OpcodeSetGlobal,
                                 0,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(vm.InternString("abc")),
                                 Value(123.0)
                         });
}

TEST(CompilerTest, GlobalVarSetFail)
{
    GenericTestChunkFail("abc * xyz = 123;");
}

TEST(CompilerTest, IfSimple)
{
    GenericTestChunkPass("if (true) print true;",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 6,
                                 OpcodePop,
                                 OpcodeTrue,
                                 OpcodePrint,
                                 OpcodeJump,
                                 0,
                                 1,
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, IfFailLeftParen)
{
    GenericTestChunkFail("if true) print true;");
}

TEST(CompilerTest, IfFailRightParen)
{
    GenericTestChunkFail("if (true print false;");
}

TEST(CompilerTest, IfFailNoCond)
{
    GenericTestChunkFail("if () print false;");
}

TEST(CompilerTest, IfFailNoStmt)
{
    GenericTestChunkFail("{if (false)}");
}

TEST(CompilerTest, IfElseFailNoStmt)
{
    GenericTestChunkFail("{if (false) print 1; else}");
}

TEST(CompilerTest, IfElse)
{
    GenericTestChunkPass("if (true) print 1; else {print 2;}",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 7,
                                 
                                 OpcodePop,
                                 OpcodePushConstant,
                                 0,
                                 OpcodePrint,
                                 OpcodeJump,
                                 0,
                                 4,
                                 
                                 OpcodePop,
                                 OpcodePushConstant,
                                 1,
                                 OpcodePrint,
                                 
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                                 Value(1.0),
                                 Value(2.0)
                         });
}

TEST(CompilerTest, And)
{
    GenericTestChunkPass("true and false;",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 2,
                                 OpcodePop,
                                 OpcodeFalse,
                                 
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, Or)
{
    GenericTestChunkPass("true or false;",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 3,
                                 
                                 OpcodeJump,
                                 0,
                                 1,
                                 
                                 OpcodeFalse,
                                 
                                 OpcodePop,
                                 OpcodeNil, OpcodeReturn
                         },
                         {
                         });
}

TEST(CompilerTest, While)
{
    GenericTestChunkPass("while(true) {print false;}",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 6,
                                 
                                 OpcodePop,
                                 
                                 OpcodeFalse,
                                 OpcodePrint,
                                 
                                 OpcodeLoop,
                                 0,
                                 10,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, WhileFailLeftParen)
{
    GenericTestChunkFail("while true) print 1;");
}

TEST(CompilerTest, WhileFailRightParen)
{
    GenericTestChunkFail("while (true print 1;");
}

TEST(CompilerTest, WhileNoCondFail)
{
    GenericTestChunkFail("while print 1;");
}

TEST(CompilerTest, WhileSimpleBreak)
{
    GenericTestChunkPass("while(true) {break;}",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 7,
                                 
                                 OpcodePop,
                                 
                                 OpcodeJump,
                                 0,
                                 4,
                                 
                                 OpcodeLoop,
                                 0,
                                 11,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, WhileBreakAndLocals)
{
    GenericTestChunkPass("while(true) {var a; {var b; break;}}",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 13,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil,
                                 OpcodeNil,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 OpcodeJump,
                                 0,
                                 6,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 
                                 OpcodeLoop,
                                 0,
                                 17,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, WhileBreakAndLocalsDontEatTooMany)
{
    GenericTestChunkPass("{var a; while(true) {var b; {var c; break;}}}",
                         {
                                 OpcodeNil,
                                 
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 13,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil,
                                 
                                 OpcodeNil,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 OpcodeJump,
                                 0,
                                 6,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 
                                 OpcodeLoop,
                                 0,
                                 17,
                                 
                                 OpcodePop,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, BreakOutside)
{
    GenericTestChunkFail("break;");
}

TEST(CompilerTest, ContinueOutside)
{
    GenericTestChunkFail("continue;");
}

TEST(CompilerTest, WhileSimpleContinue)
{
    GenericTestChunkPass("while(true) {continue;}",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 7,
                                 
                                 OpcodePop,
                                 
                                 OpcodeLoop,
                                 0,
                                 8,
                                 
                                 OpcodeLoop,
                                 0,
                                 11,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         },
                         {});
}

TEST(CompilerTest, WhileContinueAndLocals)
{
    GenericTestChunkPass("while(true) {var a; {var b; continue;}}",
                         {
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 13,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil,
                                 OpcodeNil,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 OpcodeLoop,
                                 0,
                                 12,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 
                                 OpcodeLoop,
                                 0,
                                 17,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

TEST(CompilerTest, WhileContinueAndLocalsDontEatTooMany)
{
    GenericTestChunkPass("{var a; while(true) {var b; {var c; continue;}}}",
                         {
                                 OpcodeNil,
                                 
                                 OpcodeTrue,
                                 OpcodeJumpIfFalse,
                                 0,
                                 13,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil,
                                 
                                 OpcodeNil,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 OpcodeLoop,
                                 0,
                                 12,
                                 
                                 OpcodePop,
                                 OpcodePop,
                                 
                                 OpcodeLoop,
                                 0,
                                 17,
                                 
                                 OpcodePop,
                                 
                                 OpcodePop,
                                 
                                 OpcodeNil, OpcodeReturn
                         }, {});
}

void ExpectValidScript(GcPtr<Closure> script);

GcPtr<Closure>
GenericCompileAndTestScriptCode(std::string_view input, const std::vector<uint8_t>& code, bool shouldFail);

TEST(CompilerTest, EmptyFunction)
{
    GcPtr<Closure> script = GenericCompileAndTestScriptCode("fun abc(){}",
                                                            {
                                                                     OpcodePushConstant,
                                                                     0,
                                                                     OpcodeDefineGlobal,
                                                                     1,
                                                                     
                                                                     OpcodeNil,
                                                                     OpcodeReturn
                                                             },
                                                            false);
    
    if (script.IsNullptr())
    {
        return;
    }
    
    const Chunk& chunk = script->GetChunk();
    
    EXPECT_EQ(chunk.GetConstantsCount(), 2);
    if (chunk.GetConstantsCount() != 2)
    {
        return;
    }
    
    Value first = chunk.GetConstant(0);
    Value second = chunk.GetConstant(1);
    
    EXPECT_TRUE(first.IsObject<Closure>());
    if (first.IsObject<Closure>())
    {
        GcPtr<Closure> func = first.AsObject<Closure>();
        
        EXPECT_EQ(func->GetName(), vm.InternString("abc"));
        EXPECT_EQ(func->GetArity(), 0);
        EXPECT_EQ(std::vector<uint8_t>(func->GetChunk().CodeBegin(), func->GetChunk().CodeEnd()),
                  (std::vector<uint8_t>{
                          OpcodeNil,
                          OpcodeReturn
                  }));
    }
    
    EXPECT_TRUE(second.IsObject<String>());
    if (second.IsObject<String>())
    {
        EXPECT_EQ(second.AsObject<String>(), vm.InternString("abc"));
    }
}

// TODO: FUNCTIONS TESTS.
// I don't know how to do it...

TEST(CompilerTest, ReturnAtScriptLevel)
{
    GenericTestChunkFail("return 1;");
}

GcPtr<Closure>
GenericCompileAndTestScriptCode(std::string_view input, const std::vector<uint8_t>& code, bool shouldFail)
{
    GcPtr<Closure> script = Compile(vm, errorReporter, "no-file", input);
    
    if (!shouldFail)
    {
        ExpectValidScript(script);
    }
    
    if (script.IsNullptr())
    {
        return script;
    }
    
    EXPECT_EQ(std::vector<uint8_t>(script->GetChunk().CodeBegin(), script->GetChunk().CodeEnd()), code);
    
    return script;
}

void ExpectValidScript(GcPtr<Closure> script)
{
    EXPECT_EQ(script.IsNullptr(), false);
    
    if (script.IsNullptr())
    {
        return;
    }
    
    EXPECT_EQ(script->GetName(), vm.InternString("<script>"));
    EXPECT_EQ(script->GetArity(), 0);
}

void GenericTestChunkPass(std::string_view input, const std::vector<uint8_t>& bytes, std::vector<Value> constants)
{
    GenericTestChunk(input, bytes, std::move(constants), false);
}

void GenericTestChunkFail(std::string_view input)
{
    GenericTestChunk(input, {}, {}, true);
}

void GenericTestChunk(std::string_view input, const std::vector<uint8_t>& bytes, std::vector<Value> constants,
                      bool shouldFail)
{
    GcPtr<Closure> script = GenericCompileAndTestScriptCode(input, bytes, shouldFail);
    
    if (script.IsNullptr())
    {
        return;
    }
    
    const Chunk& chunk = script->GetChunk();
    
    EXPECT_EQ(constants.size(), chunk.GetConstantsCount());
    
    for (std::size_t index = 0; index < chunk.GetConstantsCount() && index < constants.size(); ++index)
    {
        EXPECT_EQ(chunk.GetConstant(index), constants[index]);
    }
}
