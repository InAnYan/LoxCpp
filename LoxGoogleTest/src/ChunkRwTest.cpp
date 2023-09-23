#include <gtest/gtest.h>

#include <random>

#include <Lox/Lox.hpp>

using namespace Lox;

void GenericWriteRead(const std::vector<uint8_t>& bytes, const std::vector<Value>& constants);

TEST(ChunkRwTest, First)
{
    GenericWriteRead({ 0, 1, 2, 3 },
                     { NIL, Value(true), Value(false), Value(3.14) });
}

static VirtualMachineConfiguration conf(std::cout, std::cin, std::cout);
static VirtualMachine vm(conf);

TEST(ChunkRwTest, Strings)
{
    GenericWriteRead({ 0, 0 },
                     { Value(vm.InternString("abc")) });
}

void GenericWriteRead(const std::vector<uint8_t>& bytes, const std::vector<Value>& constants)
{
    Chunk chunk;
    
    std::size_t count = 1;
    for (auto byte: bytes)
    {
        chunk.PushCode(byte, count++);
    }
    
    for (auto constant: constants)
    {
        chunk.PushConstant(constant, count++);
    }
    
    std::stringstream ss;
    
    ChunkWriter writer(chunk, ss);
    writer.Write();
    EXPECT_FALSE(ss.fail());
    
    std::unique_ptr<Chunk> readChunk = ReadChunk(vm, ss);
    
    EXPECT_TRUE(readChunk != nullptr);
    
    if (!readChunk)
    {
        return;
    }
    
    EXPECT_EQ(chunk.GetCodeSize(), readChunk->GetCodeSize());
    EXPECT_EQ(chunk.GetConstantsCount(), readChunk->GetConstantsCount());
    
    auto chunkCode = std::vector<uint8_t>(chunk.CodeBegin(), chunk.CodeEnd());
    auto readChunkCode = std::vector<uint8_t>(readChunk->CodeBegin(), readChunk->CodeEnd());
    
    EXPECT_EQ(chunkCode, readChunkCode);
    
    auto chunkConstants = std::vector<Value>(chunk.ConstantsBegin(), chunk.ConstantsEnd());
    auto readChunkConstants = std::vector<Value>(readChunk->ConstantsBegin(), readChunk->ConstantsEnd());
    
    EXPECT_EQ(chunkConstants, readChunkConstants);
}
