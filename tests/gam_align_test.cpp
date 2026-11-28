/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_align.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

TEST(UtilAlignTest, IsPowerOfTwo)
{
    EXPECT_TRUE(gam::util::isPowerOfTwo(1));
    EXPECT_TRUE(gam::util::isPowerOfTwo(2));
    EXPECT_TRUE(gam::util::isPowerOfTwo(4));
    EXPECT_TRUE(gam::util::isPowerOfTwo(8));
    EXPECT_TRUE(gam::util::isPowerOfTwo(4096));

    EXPECT_FALSE(gam::util::isPowerOfTwo(0));
    EXPECT_FALSE(gam::util::isPowerOfTwo(3));
    EXPECT_FALSE(gam::util::isPowerOfTwo(6));
    EXPECT_FALSE(gam::util::isPowerOfTwo(7));
    EXPECT_FALSE(gam::util::isPowerOfTwo(12));
}

TEST(UtilAlignTest, IsAlignedIntegers)
{
    EXPECT_TRUE(gam::util::isAligned(std::size_t{0}, std::size_t{16}));
    EXPECT_TRUE(gam::util::isAligned(std::size_t{16}, std::size_t{16}));
    EXPECT_TRUE(gam::util::isAligned(std::size_t{32}, std::size_t{16}));
    EXPECT_TRUE(gam::util::isAligned(std::size_t{8}, std::size_t{8}));

    EXPECT_FALSE(gam::util::isAligned(std::size_t{15}, std::size_t{16}));
    EXPECT_FALSE(gam::util::isAligned(std::size_t{17}, std::size_t{16}));
    EXPECT_FALSE(gam::util::isAligned(std::size_t{1}, std::size_t{8}));
}

TEST(UtilAlignTest, AlignDownIntegers)
{
    EXPECT_EQ(gam::util::alignDown(std::size_t{0}, std::size_t{8}), 0u);
    EXPECT_EQ(gam::util::alignDown(std::size_t{16}, std::size_t{16}), 16u);
    EXPECT_EQ(gam::util::alignDown(std::size_t{17}, std::size_t{16}), 16u);
    EXPECT_EQ(gam::util::alignDown(std::size_t{31}, std::size_t{16}), 16u);
    EXPECT_EQ(gam::util::alignDown(std::size_t{4097}, std::size_t{4096}), 4096u);
}

TEST(UtilAlignTest, AlignUpIntegers)
{
    EXPECT_EQ(gam::util::alignUp(std::size_t{0}, std::size_t{8}), 0u);
    EXPECT_EQ(gam::util::alignUp(std::size_t{16}, std::size_t{16}), 16u);
    EXPECT_EQ(gam::util::alignUp(std::size_t{17}, std::size_t{16}), 32u);
    EXPECT_EQ(gam::util::alignUp(std::size_t{31}, std::size_t{16}), 32u);
    EXPECT_EQ(gam::util::alignUp(std::size_t{4097}, std::size_t{4096}), 8192u);
}

TEST(UtilAlignTest, PointerAlignment)
{
    alignas(16) unsigned char buffer[64];

    EXPECT_TRUE(gam::util::isAligned(buffer, 16));
    EXPECT_FALSE(gam::util::isAligned(buffer + 1, 16));

    EXPECT_EQ(gam::util::alignDown(static_cast<void*>(buffer + 1), 16), static_cast<void*>(buffer));
    EXPECT_EQ(gam::util::alignDown(static_cast<const void*>(buffer + 17), 16), static_cast<const void*>(buffer + 16));

    EXPECT_EQ(gam::util::alignUp(static_cast<void*>(buffer), 16), static_cast<void*>(buffer));
    EXPECT_EQ(gam::util::alignUp(static_cast<void*>(buffer + 1), 16), static_cast<void*>(buffer + 16));
    EXPECT_EQ(gam::util::alignUp(static_cast<const void*>(buffer + 17), 16), static_cast<const void*>(buffer + 32));
}

TEST(UtilAlignTest, ConstexprIntegerHelpers)
{
    static_assert(gam::util::isPowerOfTwo(8), "8 is a power of two");
    static_assert(!gam::util::isPowerOfTwo(3), "3 is not a power of two");
    static_assert(gam::util::isAligned(std::size_t{32}, std::size_t{16}), "32 is 16-byte aligned");
    static_assert(gam::util::alignDown(std::size_t{17}, std::size_t{16}) == 16, "17 aligns down to 16");
    static_assert(gam::util::alignUp(std::size_t{17}, std::size_t{16}) == 32, "17 aligns up to 32");

    SUCCEED();
}
