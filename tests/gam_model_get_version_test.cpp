/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include <opengam.h>

#include <gtest/gtest.h>

TEST(GamModelGetVersion, NullPointerReturnsInvalidValue)
{
    EXPECT_EQ(gamModelGetVersion(nullptr), GAM_ERROR_INVALID_VALUE);
}

TEST(GamModelGetVersion, ValidPointerWritesPackedVersion)
{
    int version = -1;
    ASSERT_EQ(gamModelGetVersion(&version), GAM_SUCCESS);
    EXPECT_EQ(version, 10);
    EXPECT_EQ(version, GAM_MODEL_VERSION);
}
