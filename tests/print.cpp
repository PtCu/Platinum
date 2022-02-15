#include <gtest/gtest.h>
#include <core/utilities.h>
using namespace platinum;

TEST(StringPrintf, Basics)
{
    EXPECT_EQ(StringPrintf("Hello, world"), "Hello, world");
    EXPECT_EQ(StringPrintf("x = %d", 5), "x = 5");
    EXPECT_EQ(StringPrintf("%f, %f, %f", 1., 1.5, -8.125),
              "1, 1.5, -8.125");
#ifndef NDEBUG
    EXPECT_DEATH(StringPrintf("not enough %s"), ".*Check failed: .*");
#endif
}

