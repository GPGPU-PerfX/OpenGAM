/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_log.h"

#include <gtest/gtest.h>

#include <cstdio>
#include <cstdlib>
#include <fstream>
#include <iterator>
#include <string>
#include <utility>
#include <vector>

namespace
{

struct Capture
{
    std::vector<std::pair<opengam::util::LogLevel, std::string>> records;
};

void captureCallback(opengam::util::LogLevel level, const char* message, void* user)
{
    auto* capture = static_cast<Capture*>(user);
    capture->records.emplace_back(level, message != nullptr ? message : "");
}

class LogTest : public ::testing::Test
{
protected:
    void TearDown() override
    {
        opengam::util::unregisterLogCallback();
        ::unsetenv("OPENGAM_LOG_DEST");
    }
};

std::string readFile(const std::string& path)
{
    std::ifstream in(path);
    return std::string((std::istreambuf_iterator<char>(in)), std::istreambuf_iterator<char>());
}

} // namespace

TEST_F(LogTest, MacrosAreNoOpsInReleaseOrEmitInDebug)
{
    Capture capture;
    opengam::util::registerLogCallback(captureCallback, &capture);

    const int before    = 0;
    int       evaluated = before;

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

    const std::string& error = capture.records[0].second;
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

TEST_F(LogTest, UnregisterLogCallbackClearsCallback)
{
    Capture first;
    Capture second;

    opengam::util::registerLogCallback(captureCallback, &first);
    opengam::util::unregisterLogCallback();
    opengam::util::registerLogCallback(captureCallback, &second);

    GAM_LOG_INFO("after-unregister");

#if defined(GAM_ENABLE_DEBUG)
    EXPECT_TRUE(first.records.empty());
    ASSERT_EQ(second.records.size(), 1u);
    EXPECT_NE(second.records[0].second.find("after-unregister"), std::string::npos);
#else
    EXPECT_TRUE(first.records.empty());
    EXPECT_TRUE(second.records.empty());
#endif
}

TEST_F(LogTest, DumpLogsToMemoryCopiesAccumulatedRecords)
{
    std::size_t needed = 0;
    ASSERT_TRUE(opengam::util::dumpLogsToMemory(nullptr, &needed));

    GAM_LOG_INFO("dump-memory-token");

    std::size_t after = 0;
    ASSERT_TRUE(opengam::util::dumpLogsToMemory(nullptr, &after));

#if defined(GAM_ENABLE_DEBUG)
    EXPECT_GT(after, needed);

    std::string buffer(after + 1, '\0');
    std::size_t size = buffer.size();
    ASSERT_TRUE(opengam::util::dumpLogsToMemory(buffer.data(), &size));
    EXPECT_EQ(size, after);
    EXPECT_NE(buffer.find("dump-memory-token"), std::string::npos);
    EXPECT_EQ(buffer.find('\033'), std::string::npos);
#else
    EXPECT_EQ(after, needed);

    EXPECT_FALSE(opengam::util::dumpLogsToMemory(nullptr, nullptr));
#endif

    EXPECT_FALSE(opengam::util::dumpLogsToMemory(nullptr, nullptr));
}

TEST_F(LogTest, DumpLogsToFileWritesAccumulatedRecords)
{
    GAM_LOG_WARN("dump-file-token");

    const std::string path = testing::TempDir() + "opengam-dump-logs.txt";
    std::remove(path.c_str());

#if defined(GAM_ENABLE_DEBUG)
    ASSERT_TRUE(opengam::util::dumpLogsToFile(path.c_str()));
    const std::string contents = readFile(path);
    EXPECT_NE(contents.find("dump-file-token"), std::string::npos);
    EXPECT_EQ(contents.find('\033'), std::string::npos);
#else
    ASSERT_TRUE(opengam::util::dumpLogsToFile(path.c_str()));
    const std::string contents = readFile(path);
    EXPECT_EQ(contents.find("dump-file-token"), std::string::npos);
#endif

    EXPECT_FALSE(opengam::util::dumpLogsToFile(nullptr));
    EXPECT_FALSE(opengam::util::dumpLogsToFile(""));
    std::remove(path.c_str());
}

TEST_F(LogTest, EnvDestFileReceivesLiveRecords)
{
    const std::string path = testing::TempDir() + "opengam-env-log.txt";
    std::remove(path.c_str());

    const std::string dest = "file=" + path;
    ASSERT_EQ(::setenv("OPENGAM_LOG_DEST", dest.c_str(), 1), 0);

    GAM_LOG_ERROR("env-file-token");

#if defined(GAM_ENABLE_DEBUG)
    const std::string contents = readFile(path);
    EXPECT_NE(contents.find("env-file-token"), std::string::npos);
    EXPECT_EQ(contents.find('\033'), std::string::npos);
#else
    std::ifstream in(path);
    EXPECT_FALSE(in.good() && in.peek() != std::char_traits<char>::eof());
#endif

    std::remove(path.c_str());
}
