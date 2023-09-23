#include <gtest/gtest.h>

#include <Lox/Lox.hpp>

std::string PrintValue(Lox::Value val);

template <typename T>
void GenericValueTest(T actual, std::string_view actualPrint);

TEST(ValueRepr, NilRepr)
{
    GenericValueTest(Lox::Nil(), "nil");
}

TEST(ValueRepr, TrueBoolRepr)
{
    GenericValueTest(true, "true");
}

TEST(ValueRepr, FalseBoolRepr)
{
    GenericValueTest(false, "false");
}

TEST(ValueRepr, DoublesRepr)
{
    GenericValueTest(3.14, "3.14");
}

template <typename T>
void GenericValueTest(T actual, std::string_view actualPrint)
{
    Lox::Value val(actual);
    
    EXPECT_TRUE(val.Is<T>());
    EXPECT_EQ(val.As<T>(), actual);
    
    EXPECT_EQ(PrintValue(val), actualPrint);
}

std::string PrintValue(Lox::Value val)
{
    std::stringstream ss;
    ss << val;
    return ss.str();
}

