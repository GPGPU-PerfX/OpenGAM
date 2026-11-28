/*
 * Copyright 2026 Tianxiang Luo <JeremyLuo007@163.com>
 * SPDX-License-Identifier: Apache-2.0
 */

#include "gam_log.h"

#include <cctype>
#include <cstdarg>
#include <cstdio>
#include <cstdlib>
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

LogCallback           g_callback  = nullptr;
void*                 g_user      = nullptr;
constexpr const char* kColorReset = "\033[0m";

constexpr std::size_t kLogDumpCapacity = 64 * 1024;

char        g_dump[kLogDumpCapacity];
std::size_t g_dumpUsed = 0;

// =====================================================================================================================
const char* levelTag(LogLevel level)
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

// =====================================================================================================================
const char* levelColor(LogLevel level)
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

// =====================================================================================================================
const char* fileName(const char* path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return "?";
    }

    const char* slash = std::strrchr(path, '/');
    if (slash != nullptr && slash[1] != '\0')
    {
        return slash + 1;
    }

    return path;
}

// =====================================================================================================================
void dropOldestRecord()
{
    if (g_dumpUsed == 0)
    {
        return;
    }

    const char* newline = static_cast<const char*>(std::memchr(g_dump, '\n', g_dumpUsed));
    if (newline == nullptr)
    {
        g_dumpUsed = 0;
        return;
    }

    const std::size_t drop = static_cast<std::size_t>(newline - g_dump) + 1;
    g_dumpUsed -= drop;
    if (g_dumpUsed > 0)
    {
        std::memmove(g_dump, g_dump + drop, g_dumpUsed);
    }
}

// =====================================================================================================================
void appendDump(const char* message)
{
    if (message == nullptr)
    {
        return;
    }

    const std::size_t textLen = std::strlen(message);
    const std::size_t need    = textLen + 1;

    if (need > kLogDumpCapacity)
    {
        const std::size_t keep = kLogDumpCapacity - 1;
        std::memcpy(g_dump, message + (textLen - keep), keep);
        g_dump[keep] = '\n';
        g_dumpUsed   = kLogDumpCapacity;
        return;
    }

    while (g_dumpUsed + need > kLogDumpCapacity)
    {
        dropOldestRecord();
    }

    std::memcpy(g_dump + g_dumpUsed, message, textLen);
    g_dump[g_dumpUsed + textLen] = '\n';
    g_dumpUsed += need;
}

// =====================================================================================================================
void skipSpaces(const char*& cursor)
{
    while (*cursor != '\0' && std::isspace(static_cast<unsigned char>(*cursor)) != 0)
    {
        ++cursor;
    }
}

// =====================================================================================================================
bool tokenEquals(const char* begin, const char* end, const char* expected)
{
    const std::size_t length = static_cast<std::size_t>(end - begin);
    return std::strlen(expected) == length && std::strncmp(begin, expected, length) == 0;
}

// =====================================================================================================================
void writeStream(FILE* stream, int fd, LogLevel level, const char* message)
{
    const bool color = (::isatty(fd) != 0);

    if (color)
    {
        std::fprintf(stream, "%s%s%s\n", levelColor(level), message, kColorReset);
    }
    else
    {
        std::fprintf(stream, "%s\n", message);
    }

    std::fflush(stream);
}

// =====================================================================================================================
void writeFilePath(const char* path, const char* message)
{
    if (path == nullptr || path[0] == '\0')
    {
        return;
    }

    FILE* file = std::fopen(path, "a");
    if (file == nullptr)
    {
        return;
    }

    std::fprintf(file, "%s\n", message);
    std::fflush(file);
    std::fclose(file);
}

// =====================================================================================================================
void writeLiveOutputs(LogLevel level, const char* message)
{
    const char* dest     = std::getenv("OPENGAM_LOG_DEST");
    bool        toStderr = true;
    bool        toStdout = true;

    if (dest != nullptr && dest[0] != '\0')
    {
        toStderr = false;
        toStdout = false;
    }

    const char* cursor = (dest != nullptr) ? dest : "";

    while (dest != nullptr && dest[0] != '\0' && *cursor != '\0')
    {
        skipSpaces(cursor);
        if (*cursor == '\0')
        {
            break;
        }

        const char* begin = cursor;
        while (*cursor != '\0' && *cursor != ',')
        {
            ++cursor;
        }

        const char* end = cursor;
        while (end > begin && std::isspace(static_cast<unsigned char>(end[-1])) != 0)
        {
            --end;
        }

        if (tokenEquals(begin, end, "stderr"))
        {
            toStderr = true;
        }
        else if (tokenEquals(begin, end, "stdout"))
        {
            toStdout = true;
        }
        else if (end > begin + 5 && std::strncmp(begin, "file=", 5) == 0)
        {
            const std::size_t pathLen = static_cast<std::size_t>(end - (begin + 5));
            if (pathLen > 0 && pathLen < 4096)
            {
                char path[4096];
                std::memcpy(path, begin + 5, pathLen);
                path[pathLen] = '\0';
                writeFilePath(path, message);
            }
        }

        if (*cursor == ',')
        {
            ++cursor;
        }
    }

    if (toStderr)
    {
        writeStream(stderr, STDERR_FILENO, level, message);
    }

    if (toStdout)
    {
        writeStream(stdout, STDOUT_FILENO, level, message);
    }
}

} // namespace

// =====================================================================================================================
void registerLogCallback(LogCallback callback, void* user)
{
    pthread_mutex_lock(&g_mutex);
    g_callback = callback;
    g_user     = user;
    pthread_mutex_unlock(&g_mutex);
}

// =====================================================================================================================
void unregisterLogCallback()
{
    registerLogCallback(nullptr, nullptr);
}

// =====================================================================================================================
bool dumpLogsToMemory(char* buffer, std::size_t* size)
{
    if (size == nullptr)
    {
        return false;
    }

    pthread_mutex_lock(&g_mutex);

    if (buffer == nullptr)
    {
        *size = g_dumpUsed;
        pthread_mutex_unlock(&g_mutex);
        return true;
    }

    if (*size == 0)
    {
        pthread_mutex_unlock(&g_mutex);
        return true;
    }

    std::size_t copy = g_dumpUsed;
    if (copy >= *size)
    {
        copy = *size - 1;

        while (copy > 0 && g_dump[copy - 1] != '\n')
        {
            --copy;
        }
    }

    if (copy > 0)
    {
        std::memcpy(buffer, g_dump, copy);
    }

    buffer[copy] = '\0';
    *size        = copy;
    pthread_mutex_unlock(&g_mutex);
    return true;
}

// =====================================================================================================================
bool dumpLogsToFile(const char* path)
{
    if (path == nullptr || path[0] == '\0')
    {
        return false;
    }

    FILE* file = std::fopen(path, "w");
    if (file == nullptr)
    {
        return false;
    }

    pthread_mutex_lock(&g_mutex);
    const bool ok = (std::fwrite(g_dump, 1, g_dumpUsed, file) == g_dumpUsed);
    pthread_mutex_unlock(&g_mutex);

    const int closed = std::fclose(file);
    return ok && closed == 0;
}

// =====================================================================================================================
void log(LogLevel level, const char* file, int line, const char* function, const char* fmt, ...)
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

    const char* func = (function != nullptr && function[0] != '\0') ? function : "?";

    char message[1280];
    std::snprintf(message, sizeof(message), "[%s] %s:%d %s: %s", levelTag(level), fileName(file), line, func, body);

    pthread_mutex_lock(&g_mutex);

    LogCallback callback = g_callback;
    void*       user     = g_user;

    appendDump(message);
    writeLiveOutputs(level, message);

    if (callback != nullptr)
    {
        callback(level, message, user);
    }

    pthread_mutex_unlock(&g_mutex);
}

} // namespace util
} // namespace opengam
