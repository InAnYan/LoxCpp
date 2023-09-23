#include <gtest/gtest.h>

#include <Lox/Lox.hpp>

using namespace Lox;

static std::stringstream testOutput;
static VirtualMachineConfiguration conf(testOutput, std::cin, std::cout);
static VirtualMachine vm(conf);

void GenericTest(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants,
                 std::string_view outputShould);

TEST(VmTest, OnlyReturn)
{
    GenericTest(
            {
                    OpcodeNil,
                    OpcodeReturn,
            },
            {},
            "");
}

TEST(VmTest, BasicsAndPushConstant)
{
    GenericTest(
            {
                    OpcodePushConstant,
                    0,
                    OpcodePrint,
                    OpcodeNil,
                    OpcodeReturn,
            },
            {
                    Value(3.14)
                
            },
            "3.14\n");
}

template <typename Num>
void GenericBinaryTest(Opcode opcode, Num a, Num b, std::string_view result)
{
    GenericTest(
            {
                    OpcodePushConstant,
                    0,
                    OpcodePushConstant,
                    1,
                    opcode,
                    OpcodePrint,
                    OpcodeNil,
                    OpcodeReturn,
            },
            {
                    Value(a),
                    Value(b),
            },
            result);
}

TEST(VmTest, DoubleAdd)
{
    GenericBinaryTest(OpcodeAdd, 2.0, 3.0, "5\n");
}

TEST(VmTest, DoubeSubstract)
{
    GenericBinaryTest(OpcodeSubstract, 7.0, 3.0, "4\n");
}

TEST(VmTest, DoubleMultiply)
{
    GenericBinaryTest(OpcodeMultiply, 8.0, 10.0, "80\n");
}

TEST(VmTest, DoubleDivide)
{
    GenericBinaryTest(OpcodeDivide, 9.0, 3.0, "3\n");
}

TEST(VmTest, DoubleZeroDivision)
{
    EXPECT_THROW(GenericBinaryTest(OpcodeDivide, 6.0, 0.0, "");, Exceptions::ZeroDivision);
}

void GenericFastValuePush(Opcode opcode, std::string_view result);

TEST(VmTest, Nil)
{
    GenericFastValuePush(OpcodeNil, "nil\n");
}

TEST(VmTest, True)
{
    GenericFastValuePush(OpcodeTrue, "true\n");
}

TEST(VmTest, False)
{
    GenericFastValuePush(OpcodeFalse, "false\n");
}

void GenericNotTest(Value val, bool boolean);

TEST(VmTest, NumberNot)
{
    GenericNotTest(Value(10.0), false);
}

TEST(VmTest, BoolNotTrue)
{
    GenericNotTest(Value(true), false);
}

TEST(VmTest, BoolNotFalse)
{
    GenericNotTest(Value(false), true);
}

TEST(VmTest, NilNot)
{
    GenericNotTest(NIL, true);
}

TEST(VmTest, DoublesNegate)
{
    GenericTest({
                        OpcodePushConstant,
                        0,
                        OpcodeNegate,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                { Value(2.0) },
                "-2\n");
}

void GenericEqual(Value a, Value b, bool isEqual);

TEST(VmTest, EqualNumberPass)
{
    GenericEqual(Value(10.0), Value(10.0), true);
}

TEST(VmTest, EqualNumberFail)
{
    GenericEqual(NIL, Value(666.6), false);
    GenericEqual(Value(true), Value(0.5), false);
    GenericEqual(Value(false), Value(123.0), false);
    GenericEqual(Value(13.0), Value(85.0), false);
}

TEST(VmTest, EqualBoolPass)
{
    GenericEqual(Value(true), Value(true), true);
    GenericEqual(Value(false), Value(false), true);
}

TEST(VmTest, EqualBoolFail)
{
    GenericEqual(NIL, Value(false), false);
    GenericEqual(NIL, Value(true), false);
    GenericEqual(Value(true), Value(false), false);
    GenericEqual(Value(true), Value(false), false);
    GenericEqual(Value(3.14), Value(false), false);
    GenericEqual(Value(8.54), Value(true), false);
}

TEST(VmTest, EqualNilPass)
{
    GenericEqual(NIL, NIL, true);
}

TEST(VmTest, EqualNilFail)
{
    GenericEqual(NIL, Value(true), false);
    GenericEqual(NIL, Value(false), false);
    GenericEqual(NIL, Value(124324.32912), false);
}

void GenericComparison(Opcode opcode, Value a, Value b, bool isTrue);

TEST(VmTest, Comparison)
{
    GenericComparison(OpcodeGreater, Value(13.0), Value(10.0), true);
    GenericComparison(OpcodeGreater, Value(8.0), Value(15.0), false);
    GenericComparison(OpcodeLess, Value(13.0), Value(10.0), false);
    GenericComparison(OpcodeLess, Value(8.0), Value(15.0), true);
}

TEST(VmTest, ComparisonWrongType)
{
    EXPECT_THROW(GenericComparison(OpcodeGreater, NIL, Value(false), false);, Exceptions::WrongTypeValue);
    EXPECT_THROW(GenericComparison(OpcodeLess, Value(false), NIL, false);, Exceptions::WrongTypeValue);
}

TEST(VmTest, StringConc)
{
    GenericTest({
                        OpcodePushConstant,
                        0,
                        OpcodePushConstant,
                        1,
                        OpcodeAdd,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {
                        Value(vm.InternString("str")),
                        Value(vm.InternString("123"))
                },
                "str123\n");
}

TEST(VmTest, Pop)
{
    GenericTest({
                        OpcodeTrue,
                        OpcodeFalse,
                        OpcodePop,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {},
                "true\n");
}

TEST(VmTest, DefineAndGetGlobal)
{
    GenericTest({
                        OpcodeTrue,
                        OpcodeDefineGlobal,
                        0,
                        OpcodeGetGlobal,
                        0,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {
                        Value(vm.InternString("test_global"))
                },
                "true\n");
}

TEST(VmTest, DefineAndGetGlobalAndInterning)
{
    GenericTest({
                        OpcodeTrue,
                        OpcodeDefineGlobal,
                        0,
                        OpcodeGetGlobal,
                        1,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {
                        Value(vm.InternString("test_global")),
                        Value(vm.InternString("test_global"))
                },
                "true\n");
}

TEST(VmTest, DefineAndSetGlobal)
{
    GenericTest({
                        OpcodeFalse,
                        OpcodeDefineGlobal,
                        0,
                        OpcodeTrue,
                        OpcodeSetGlobal,
                        0,
                        OpcodePop,
                        OpcodeGetGlobal,
                        0,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {
                        Value(vm.InternString("xyz"))
                },
                "true\n");
}

TEST(VmTest, EndLocalNothing)
{
    GenericTest({
                        OpcodeTrue,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                }, {}, "true\n");
}

TEST(VmTest, GetLocal)
{
    GenericTest({
                        OpcodeFalse,
                        OpcodeGetLocal,
                        1,
                        OpcodePrint,
                        OpcodePop,
                        OpcodeNil,
                        OpcodeReturn
                }, {}, "false\n");
}

TEST(VmTest, SetLocal)
{
    GenericTest({
                        OpcodeFalse,
                        OpcodeTrue,
                        OpcodeSetLocal,
                        1,
                        OpcodePop,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                }, {}, "true\n");
}

TEST(VmTest, Jump)
{
    GenericTest(
            {
                    OpcodeTrue,
                    OpcodeJump,
                    0,
                    1,
                    OpcodeFalse,
                    OpcodePrint,
                    OpcodeNil,
                    OpcodeReturn
            },
            {},
            "true\n");
}

TEST(VmTest, JumpIfFalseJump)
{
    GenericTest({
                        OpcodeFalse,
                        OpcodeJumpIfFalse,
                        0,
                        1,
                        OpcodeTrue,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {},
                "false\n");
}

TEST(VmTest, JumpIfFalseNoJump)
{
    GenericTest({
                        OpcodeTrue,
                        OpcodeJumpIfFalse,
                        0,
                        2,
                        OpcodePop,
                        OpcodeFalse,
                        OpcodePrint,
                        OpcodeNil,
                        OpcodeReturn
                },
                {},
                "false\n");
}

TEST(VmTest, Loop)
{
    GenericTest(
            {
                    OpcodeJump,
                    0,
                    4,
                    OpcodeTrue,
                    OpcodeJump,
                    0,
                    3,
                    OpcodeLoop,
                    0,
                    7,
                    OpcodePrint,
                    OpcodeNil,
                    OpcodeReturn
            },
            {},
            "true\n");
}

TEST(VmTest, CallTest)
{
    Chunk funcChunk;
    funcChunk.PushCode(OpcodeTrue, 1);
    funcChunk.PushCode(OpcodeReturn, 1);
    
    GenericTest({
                        OpcodePushConstant,
                        0,
                        OpcodeDefineGlobal,
                        1,
                        
                        OpcodeGetGlobal,
                        1,
                        OpcodeCall,
                        0,
                        OpcodePrint,
                        
                        OpcodeNil,
                        OpcodeReturn
                },
                {
                        Value(vm.AllocateObject<Closure>(vm.InternString("abc"), 0, Chunk(funcChunk))),
                        Value(vm.InternString("abc"))
                },
                "true\n");
}

// TODO: Upvalue get/set test.

void GenericComparison(Opcode opcode, Value a, Value b, bool isTrue)
{
    GenericTest({ OpcodePushConstant,
                  0,
                  OpcodePushConstant,
                  1,
                  opcode,
                  OpcodePrint,
                  OpcodeNil,
                  OpcodeReturn },
                { a, b },
                isTrue ? "true\n" : "false\n");
    
}

void GenericEqual(Value a, Value b, bool isEqual)
{
    GenericTest({ OpcodePushConstant,
                  0,
                  OpcodePushConstant,
                  1,
                  OpcodeEqual,
                  OpcodePrint,
                  OpcodeNil,
                  OpcodeReturn },
                { a, b },
                isEqual ? "true\n" : "false\n");
}

void GenericNotTest(Value val, bool boolean)
{
    GenericTest({ OpcodePushConstant,
                  0,
                  OpcodeNot,
                  OpcodePrint,
                  OpcodeNil,
                  OpcodeReturn },
                { val },
                boolean ? "true\n" : "false\n");
}

void GenericTest(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants,
                 std::string_view outputShould)
{
    GcPtr<Closure> func = vm.AllocateObject<Closure>(vm.InternString("<script>"), 0);
    
    Chunk& chunk = func->GetChunk();
    
    for (uint8_t byte: code)
    {
        chunk.PushCode(byte, 1);
    }
    
    for (auto value: constants)
    {
        chunk.PushConstant(value, 1);
    }
    
    ChunkChecker checker(chunk);
    EXPECT_EQ(checker.Check(), true);
    
    ChunkDumper dumper(chunk, "script", std::cout);
    dumper.Dump();
    
    vm.RunScript(func);
    
    std::string output = testOutput.str();
    testOutput.str(std::string());
    testOutput.clear();
    
    // TODO: Non-compatible string new line.
    EXPECT_EQ(output, outputShould);
}

void GenericFastValuePush(Opcode opcode, std::string_view result)
{
    GenericTest(
            {
                    opcode,
                    OpcodePrint,
                    OpcodeNil,
                    OpcodeReturn
            },
            {},
            result);
}

