/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_align.h"

#include <gtest/gtest.h>

#include <cstddef>
#include <cstdint>

TEST(AlignTest, IsPowerOfTwo)
{
    EXPECT_TRUE(opengam::util::isPowerOfTwo(1));
    EXPECT_TRUE(opengam::util::isPowerOfTwo(2));
    EXPECT_TRUE(opengam::util::isPowerOfTwo(4));
    EXPECT_TRUE(opengam::util::isPowerOfTwo(8));
    EXPECT_TRUE(opengam::util::isPowerOfTwo(4096));

    EXPECT_FALSE(opengam::util::isPowerOfTwo(0));
    EXPECT_FALSE(opengam::util::isPowerOfTwo(3));
    EXPECT_FALSE(opengam::util::isPowerOfTwo(6));
    EXPECT_FALSE(opengam::util::isPowerOfTwo(7));
    EXPECT_FALSE(opengam::util::isPowerOfTwo(12));
}

TEST(AlignTest, IsAlignedIntegers)
{
    EXPECT_TRUE(opengam::util::isAligned(std::size_t{0}, std::size_t{16}));
    EXPECT_TRUE(opengam::util::isAligned(std::size_t{16}, std::size_t{16}));
    EXPECT_TRUE(opengam::util::isAligned(std::size_t{32}, std::size_t{16}));
    EXPECT_TRUE(opengam::util::isAligned(std::size_t{8}, std::size_t{8}));

    EXPECT_FALSE(opengam::util::isAligned(std::size_t{15}, std::size_t{16}));
    EXPECT_FALSE(opengam::util::isAligned(std::size_t{17}, std::size_t{16}));
    EXPECT_FALSE(opengam::util::isAligned(std::size_t{1}, std::size_t{8}));
}

TEST(AlignTest, AlignDownIntegers)
{
    EXPECT_EQ(opengam::util::alignDown(std::size_t{0}, std::size_t{8}), 0u);
    EXPECT_EQ(opengam::util::alignDown(std::size_t{16}, std::size_t{16}), 16u);
    EXPECT_EQ(opengam::util::alignDown(std::size_t{17}, std::size_t{16}), 16u);
    EXPECT_EQ(opengam::util::alignDown(std::size_t{31}, std::size_t{16}), 16u);
    EXPECT_EQ(opengam::util::alignDown(std::size_t{4097}, std::size_t{4096}), 4096u);
}

TEST(AlignTest, AlignUpIntegers)
{
    EXPECT_EQ(opengam::util::alignUp(std::size_t{0}, std::size_t{8}), 0u);
    EXPECT_EQ(opengam::util::alignUp(std::size_t{16}, std::size_t{16}), 16u);
    EXPECT_EQ(opengam::util::alignUp(std::size_t{17}, std::size_t{16}), 32u);
    EXPECT_EQ(opengam::util::alignUp(std::size_t{31}, std::size_t{16}), 32u);
    EXPECT_EQ(opengam::util::alignUp(std::size_t{4097}, std::size_t{4096}), 8192u);
}

TEST(AlignTest, PointerAlignment)
{
    alignas(16) unsigned char buffer[64];

    EXPECT_TRUE(opengam::util::isAligned(buffer, 16));
    EXPECT_FALSE(opengam::util::isAligned(buffer + 1, 16));

    EXPECT_EQ(opengam::util::alignDown(static_cast<void *>(buffer + 1), 16),
              static_cast<void *>(buffer));
    EXPECT_EQ(opengam::util::alignDown(static_cast<const void *>(buffer + 17), 16),
              static_cast<const void *>(buffer + 16));

    EXPECT_EQ(opengam::util::alignUp(static_cast<void *>(buffer), 16),
              static_cast<void *>(buffer));
    EXPECT_EQ(opengam::util::alignUp(static_cast<void *>(buffer + 1), 16),
              static_cast<void *>(buffer + 16));
    EXPECT_EQ(opengam::util::alignUp(static_cast<const void *>(buffer + 17), 16),
              static_cast<const void *>(buffer + 32));
}

TEST(AlignTest, ConstexprIntegerHelpers)
{
    static_assert(opengam::util::isPowerOfTwo(8), "8 is a power of two");
    static_assert(!opengam::util::isPowerOfTwo(3), "3 is not a power of two");
    static_assert(opengam::util::isAligned(std::size_t{32}, std::size_t{16}),
                  "32 is 16-byte aligned");
    static_assert(opengam::util::alignDown(std::size_t{17}, std::size_t{16}) == 16,
                  "17 aligns down to 16");
    static_assert(opengam::util::alignUp(std::size_t{17}, std::size_t{16}) == 32,
                  "17 aligns up to 32");

    SUCCEED();
}
