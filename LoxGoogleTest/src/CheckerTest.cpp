#include <gtest/gtest.h>

#include <Lox/Lox.hpp>

using namespace Lox;

static VirtualMachineConfiguration conf(std::cout, std::cin, std::cout);
static VirtualMachine vm(conf);

void GenericCheck(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants, bool result);

void GenericCheckFail(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants);

void GenericCheckPass(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants);

/*
TEST(CheckerTest, EmptyFail)
{
    GenericCheckFail({}, {});
}

TEST(CheckerTest, OnlyReturnPass)
{
    GenericCheckPass({ OpcodeReturn }, {});
}

TEST(CheckerTest, NoSuchOpcodeFail)
{
    GenericCheckFail(
            {
                    OpcodePushConstant,
                    4,
                    OpcodePrint,
                    OpcodeReturn,
            },
            {});
}

TEST(CheckerTest, InvalidGetLocal)
{
    GenericCheckFail({
                             OpcodeGetLocal,
                             2,
                             OpcodeReturn
                     },
                     {});
}
 */

void GenericCheck(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants, bool result)
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
    
    ChunkChecker checker(chunk);
    EXPECT_EQ(checker.Check(), result);
}

void GenericCheckFail(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants)
{
    GenericCheck(code, constants, false);
}

void GenericCheckPass(std::initializer_list<uint8_t> code, std::initializer_list<Value> constants)
{
    GenericCheck(code, constants, true);
}
