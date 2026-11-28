/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_log.h"

#include <gtest/gtest.h>

#include <string>
#include <utility>
#include <vector>

namespace
{

struct Capture
{
    std::vector<std::pair<opengam::util::LogLevel, std::string>> records;
};

void captureSink(opengam::util::LogLevel level, const char *message, void *user)
{
    auto *capture = static_cast<Capture *>(user);
    capture->records.emplace_back(level, message != nullptr ? message : "");
}

class LogTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        opengam::util::resetLogSink();
    }
};

} // namespace

TEST_F(LogTest, MacrosAreNoOpsInReleaseOrEmitInDebug)
{
    Capture capture;
    opengam::util::setLogSink(captureSink, &capture);

    const int before = 0;
    int evaluated = before;

    GAM_LOG_ERROR("error-record %d", ++evaluated);
    GAM_LOG_WARN("warn-record %d", ++evaluated);
    GAM_LOG_INFO("info-record %d", ++evaluated);
    GAM_LOG_TRACE("trace-record %d", ++evaluated);

#if defined(GAM_ENABLE_DEBUG)
    ASSERT_EQ(capture.records.size(), 4u);
    EXPECT_EQ(evaluated, before + 4);

    EXPECT_EQ(capture.records[0].first, opengam::util::LogLevel::Error);
    EXPECT_EQ(capture.records[1].first, opengam::util::LogLevel::Warn);
    EXPECT_EQ(capture.records[2].first, opengam::util::LogLevel::Info);
    EXPECT_EQ(capture.records[3].first, opengam::util::LogLevel::Trace);

    const std::string &error = capture.records[0].second;
    EXPECT_NE(error.find("[ERROR]"), std::string::npos);
    EXPECT_NE(error.find("gam_log_test.cpp"), std::string::npos);
    EXPECT_NE(error.find("error-record 1"), std::string::npos);
    EXPECT_EQ(error.find('\033'), std::string::npos);

    EXPECT_NE(capture.records[1].second.find("[WARN]"), std::string::npos);
    EXPECT_NE(capture.records[1].second.find("warn-record 2"), std::string::npos);
    EXPECT_NE(capture.records[2].second.find("[INFO]"), std::string::npos);
    EXPECT_NE(capture.records[2].second.find("info-record 3"), std::string::npos);
    EXPECT_NE(capture.records[3].second.find("[TRACE]"), std::string::npos);
    EXPECT_NE(capture.records[3].second.find("trace-record 4"), std::string::npos);

    // The formatted record includes a line number and the enclosing
    // function (TestBody under GoogleTest), so a later failure can be
    // traced to this call site.
    EXPECT_NE(error.find("gam_log_test.cpp:"), std::string::npos);
    EXPECT_NE(error.find("TestBody"), std::string::npos);
#else
    EXPECT_TRUE(capture.records.empty());
    EXPECT_EQ(evaluated, before);
#endif
}

TEST_F(LogTest, ResetLogSinkRestoresDefaultSink)
{
    Capture first;
    Capture second;

    opengam::util::setLogSink(captureSink, &first);
    opengam::util::resetLogSink();
    opengam::util::setLogSink(captureSink, &second);

    GAM_LOG_INFO("after-reset");

#if defined(GAM_ENABLE_DEBUG)
    EXPECT_TRUE(first.records.empty());
    ASSERT_EQ(second.records.size(), 1u);
    EXPECT_NE(second.records[0].second.find("after-reset"), std::string::npos);
#else
    EXPECT_TRUE(first.records.empty());
    EXPECT_TRUE(second.records.empty());
#endif
}
