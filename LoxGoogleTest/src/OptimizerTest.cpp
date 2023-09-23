#include <gtest/gtest.h>

#include <Lox/Lox.hpp>

using namespace Lox;

void GenericChunkTest(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants,
                      std::initializer_list<uint8_t> newCode, std::initializer_list<Value> newConstants);

TEST(OptimizerTest, ExpressionDoublesConstantFolding)
{
    GenericChunkTest(
            {
                    OpcodePushConstant,
                    0,
                    OpcodePushConstant,
                    1,
                    OpcodeMultiply,
                    OpcodePushConstant,
                    2,
                    OpcodeAdd,
                    OpcodePushConstant,
                    3,
                    OpcodeDivide,
                    OpcodePrint,
                    OpcodeReturn,
            },
            {
                    Value(2.0),
                    Value(3.0),
                    Value(1.0),
                    Value(4.0),
            },
            {
                    OpcodePushConstant,
                    6,
                    OpcodePrint,
                    OpcodeReturn,
            },
            {
                    Value(2.0),
                    Value(3.0),
                    Value(1.0),
                    Value(4.0),
                    Value(6.0),
                    Value(7.0),
                    Value(1.75),
            });
}

TEST(OptimizerTest, Equal)
{
    GenericChunkTest({
                             OpcodeTrue,
                             OpcodeFalse,
                             OpcodeEqual,
                     },
                     {},
                     { OpcodeFalse },
                     {});
}

TEST(OptimizerTest, Not)
{
    GenericChunkTest({
                             OpcodeFalse,
                             OpcodeNot,
                             OpcodePrint,
                             OpcodeReturn },
                     {},
                     {
                             OpcodeTrue,
                             OpcodePrint,
                             OpcodeReturn
                     },
                     {});
}

TEST(OptimizerTest, Negate)
{
    GenericChunkTest({
                             OpcodePushConstant,
                             0,
                             OpcodeNegate,
                             OpcodePrint,
                             OpcodeReturn
                     },
                     { Value(3.14) },
                     {
                             OpcodePushConstant,
                             1,
                             OpcodePrint,
                             OpcodeReturn
                     },
                     
                     { Value(3.14), Value(-3.14) });
}

static VirtualMachineConfiguration conf(std::cout, std::cin, std::cout);
static VirtualMachine vm(conf);

TEST(OptimizerTest, StringConc)
{
    GenericChunkTest({
                             OpcodePushConstant,
                             0,
                             OpcodePushConstant,
                             1,
                             OpcodeAdd,
                             OpcodePushConstant,
                             2,
                             OpcodeAdd,
                             OpcodePrint,
                             OpcodeReturn
                     },
                     {
                             Value(vm.InternString("abc")),
                             Value(vm.InternString("_")),
                             Value(vm.InternString("xyz"))
                     },
                     {
                             OpcodePushConstant,
                             4,
                             OpcodePrint,
                             OpcodeReturn
                     },
                     {
                             Value(vm.InternString("abc")),
                             Value(vm.InternString("_")),
                             Value(vm.InternString("xyz")),
                             Value(vm.InternString("abc_")),
                             Value(vm.InternString("abc_xyz"))
                     });
}

void GenericChunkTest(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants,
                      std::initializer_list<uint8_t> newCode, std::initializer_list<Value> newConstants)
{
    Chunk chunk;
    
    for (uint8_t byte: code)
    {
        chunk.PushCode(byte, 1);
    }
    
    for (auto value: constants)
    {
        chunk.PushConstant(value, 1);
    }
    
    ChunkOptimizer optimizer(vm, chunk);
    optimizer.Optimize();
    
    ChunkDumper dumper(chunk, "<script>", std::cout);
    dumper.Dump();
    
    auto chunkCode = std::vector<uint8_t>(chunk.CodeBegin(), chunk.CodeEnd());
    auto newChunkCode = std::vector<uint8_t>(newCode.begin(), newCode.end());
    
    EXPECT_EQ(chunkCode, newChunkCode);
    
    auto chunkConstants = std::vector<Value>(chunk.ConstantsBegin(), chunk.ConstantsEnd());
    auto newChunkConstants = std::vector<Value>(newConstants.begin(), newConstants.end());
    
    EXPECT_EQ(chunkConstants, newChunkConstants);
}
