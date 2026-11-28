/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_assert.h"

#include "gam_log.h"

#include <gtest/gtest.h>

class UtilAssertTest : public ::testing::Test
{
protected:
    void SetUp() override { ::testing::FLAGS_gtest_death_test_style = "threadsafe"; }

    void TearDown() override { gam::util::unregisterLogCallback(); }
};

TEST_F(UtilAssertTest, MacrosAreNoOpsInReleaseOrPassWhenTrue)
{
    int evaluated = 0;

    GAM_ASSERT(++evaluated == 1);
    GAM_ASSERT_MSG(++evaluated == 2, "counter is %d", evaluated);

#if defined(OGAM_ENABLE_DEBUG)
    // The two passing checks evaluate their expressions. The failing helpers are not called here; they are covered by
    // death tests.
    EXPECT_EQ(evaluated, 2);
#else
    GAM_NOT_IMPLEMENTED();
    GAM_NEVER_REACHED();
    EXPECT_EQ(evaluated, 0);
#endif
}

#if defined(OGAM_ENABLE_DEBUG)

TEST_F(UtilAssertTest, AssertFailureLogsAndTraps)
{
    ASSERT_DEATH({ GAM_ASSERT(false); }, "assertion failed: false");
}

TEST_F(UtilAssertTest, AssertMessageFailureLogsAndTraps)
{
    ASSERT_DEATH(
        { GAM_ASSERT_MSG(1 == 2, "values %d and %d", 1, 2); }, "assertion failed: 1 == 2 \\(values 1 and 2\\)");
}

TEST_F(UtilAssertTest, NotImplementedLogsAndTraps)
{
    ASSERT_DEATH({ GAM_NOT_IMPLEMENTED(); }, "assertion failed: GAM_NOT_IMPLEMENTED");
}

TEST_F(UtilAssertTest, NeverReachedLogsAndTraps)
{
    ASSERT_DEATH({ GAM_NEVER_REACHED(); }, "assertion failed: GAM_NEVER_REACHED");
}

#endif
