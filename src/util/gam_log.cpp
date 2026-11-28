/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_log.h"

#include <cstdarg>
#include <cstdio>
#include <cstring>

#include <pthread.h>
#include <unistd.h>

namespace opengam
{
namespace util
{
namespace
{

pthread_mutex_t g_mutex = PTHREAD_MUTEX_INITIALIZER;

LogSink g_sink = nullptr;
void *g_user = nullptr;

const char *levelTag(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Error:
        return "ERROR";
    case LogLevel::Warn:
        return "WARN";
    case LogLevel::Info:
        return "INFO";
    case LogLevel::Trace:
        return "TRACE";
    }

    return "LOG";
}

const char *levelColor(LogLevel level)
{
    switch (level)
    {
    case LogLevel::Error:
        return "\033[31m";
    case LogLevel::Warn:
        return "\033[33m";
    case LogLevel::Info:
        return "\033[32m";
    case LogLevel::Trace:
        return "\033[90m";
    }

    return "\033[0m";
}

constexpr const char *kColorReset = "\033[0m";

const char *fileName(const char *path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return "?";
    }

    const char *slash = std::strrchr(path, '/');
    if (slash != nullptr && slash[1] != '\0')
    {
        return slash + 1;
    }

    return path;
}

void writeDefaultSink(LogLevel level, const char *message)
{
    const bool color = (::isatty(STDERR_FILENO) != 0);

    if (color)
    {
        std::fprintf(stderr, "%s%s%s\n", levelColor(level), message, kColorReset);
    }
    else
    {
        std::fprintf(stderr, "%s\n", message);
    }

    std::fflush(stderr);
}

} // namespace

void setLogSink(LogSink sink, void *user)
{
    pthread_mutex_lock(&g_mutex);
    g_sink = sink;
    g_user = user;
    pthread_mutex_unlock(&g_mutex);
}

void resetLogSink()
{
    setLogSink(nullptr, nullptr);
}

namespace detail
{

void log(LogLevel level,
         const char *file,
         int line,
         const char *function,
         const char *fmt,
         ...)
{
    char body[1024];
    body[0] = '\0';

    if (fmt != nullptr)
    {
        va_list args;
        va_start(args, fmt);
        std::vsnprintf(body, sizeof(body), fmt, args);
        va_end(args);
    }

    const char *func = (function != nullptr && function[0] != '\0') ? function : "?";

    char message[1280];
    std::snprintf(message,
                  sizeof(message),
                  "[%s] %s:%d %s: %s",
                  levelTag(level),
                  fileName(file),
                  line,
                  func,
                  body);

    pthread_mutex_lock(&g_mutex);

    LogSink sink = g_sink;
    void *user = g_user;

    if (sink != nullptr)
    {
        sink(level, message, user);
    }
    else
    {
        writeDefaultSink(level, message);
    }

    pthread_mutex_unlock(&g_mutex);
}

} // namespace detail

} // namespace util
} // namespace opengam
